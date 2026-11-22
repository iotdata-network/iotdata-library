/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * iotdata-generator-fields.js — pumps the field specification into the templates
 *
 * THIS PROGRAM KNOWS NO C. Every brace, guard, typedef and macro form lives in
 * generators/templates/fields.c.*.tmpl; everything true of a field in any
 * language lives in specification/iotdata-fields.xml. So a toolchain that wants
 * the C shaped differently is a template edit, and nobody has to come in here.
 *
 * THE TEMPLATE LANGUAGE IS FIVE CONSTRUCTS AND WILL STAY FIVE:
 *
 *   {{NAME}} {{name}} {{id}} {{bits}}     a value from the current scope
 *   {{#field}} … {{/field}}               define the per-field block, once
 *   {{field depth}}                       put that block here, for this field
 *   {{#each quantity|constant|check}} … {{/each}}
 *   {{#if composite|atomic|…}} … {{/if}}  presence test on the scope
 *
 * A FIELD IS PLACED BY NAME RATHER THAN ITERATED, because the template owns the
 * layout and the fields are not contiguous in the file -- CLOUDS sits between
 * SOLAR and AIR_QUALITY, DEPTH between RADIATION and POSITION. Iterating the
 * specification would emit them wherever the loop happens to be and move them.
 * Placement by name also means a vendor's extra field lands where the template
 * says, not wherever its document was concatenated. Every specified field must
 * be placed exactly once; the generator refuses otherwise, which is what catches
 * a field added to the specification and forgotten in the template.
 *   {{impl pack}}                         a body from the spec, verbatim
 *   {{doc field}}                         a note from the spec, as written
 *
 * There are no expressions, no arithmetic and no filters, because the moment
 * there are, the template becomes a program and this file becomes its
 * interpreter. ANYTHING THAT WOULD NEED A SIXTH CONSTRUCT GOES IN THE
 * SPECIFICATION AS A VERBATIM BLOCK INSTEAD. That is the rule that keeps both
 * halves readable.
 *
 * WHITESPACE: an {{#each}} concatenates its body exactly as written and adds
 * nothing, so the template author decides where the newlines are. Alignment is
 * not this program's problem -- `make generate` runs clang-format afterwards,
 * which is also what makes a byte-for-byte diff against the committed files a
 * fair test of the CONTENT rather than of our ability to pad columns.
 *
 * Usage:  node iotdata-generator-fields.js [--spec F] [--templates D] [--out D] [--stdout WHICH]
 */

'use strict';

const fs = require('fs');
const path = require('path');
const { parseXml, kid, kids, attr, hasAttr, textOf } = require('./iotdata-generator-functions-xml.js');

/* ---------------------------------------------------------------------------
 * The specification, as the template wants to see it
 * -------------------------------------------------------------------------*/

/*
 * SEVERAL DOCUMENTS, ONE SET OF FIELDS.
 *
 * The protocol's own fields are grouped by subject -- weather, device, unitary --
 * and a vendor adds fields by dropping another iotdata-fields-*.xml beside them.
 * No document declares where its fields go in the output, because the TEMPLATE
 * places fields by name; so document order is irrelevant, which is exactly what
 * a drop-in needs. Files are read in sorted order only so that an error message
 * is reproducible.
 *
 * TWO THINGS ONLY A WHOLE-SET VIEW CAN CHECK, and both are checked here:
 *
 *   - a field NAME declared twice, which would otherwise mean one document
 *     silently losing to another;
 *   - a field IDENTIFIER used twice, which is the collision that nothing in the
 *     library can catch. The forward switch in iotdata_node.h cannot: its case
 *     labels are the build-local enum ordinals, so two fields returning the same
 *     identifier compile perfectly. Two implementations would then disagree about
 *     what a variant's slot carries, with nothing anywhere saying so. This is the
 *     only place that sees every field at once, enabled or not, so this is where
 *     it has to be caught.
 */
function readSpecs(files) {
    const all = [];
    const byName = new Map();
    const byId = new Map();
    for (const file of files) {
        for (const f of readSpec(file)) {
            const prevName = byName.get(f.name);
            if (prevName) throw new Error(`field '${f.name}' is declared in both ${prevName.file} and ${file}`);
            const idKey = String(f.id).toLowerCase().replace(/u$/, '');
            const prevId = byId.get(idKey);
            if (prevId) throw new Error(`identifier ${f.id} is used by '${prevId.name}' (${prevId.file}) and '${f.name}' (${file}) -- identifiers must be unique across every specification document`);
            f.file = file;
            byName.set(f.name, f);
            byId.set(idKey, f);
            all.push(f);
        }
    }
    /* WHICH FAMILY A PART BELONGS TO IS DERIVED, not declared twice. A composite says what its
       parts are; a part therefore need not repeat which composite claims it, and the two cannot
       disagree. The membership decides the field's guards: a part's storage has to exist whenever
       EITHER it or its composite is enabled, while its slot and its ops belong to the part alone. */
    for (const f of all)
        for (const partName of f.parts) {
            const part = byName.get(partName);
            if (!part) throw new Error(`field '${f.name}' lists part '${partName}', which no document declares`);
            if (part.family) throw new Error(`field '${partName}' is claimed by both '${part.family}' and '${f.name}'`);
            part.family = f.name;
            part.family_delegates = f.delegates;
        }
    return all;
}

/* the text of an optional element, or '' when the specification did not give one */
const textOrNone = (node) => (node ? textOf(node) : '');

function readSpec(file) {
    const root = parseXml(fs.readFileSync(file, 'utf8'), file);
    if (root.tag !== 'fields') throw new Error(`${file}: root element is <${root.tag}>, expected <fields>`);

    return kids(root, 'field').map((f) => {
        const name = attr(f, 'name');
        if (name !== name.toLowerCase()) throw new Error(`field '${name}': names are lower case in the specification; the template decides where they are shouted`);

        const doc = kid(f, 'documentation');
        const notes = {};
        if (doc)
            for (const n of kids(doc, 'note')) {
                const key = attr(n, 'for');
                if (key in notes) throw new Error(`field '${name}': two <note> elements for '${key}'`);
                notes[key] = textOf(n);
            }

        /* An <impl> carries its SIGNATURE as well as its body, because a quantise_ or dequantise_
           function's return type and argument are genuinely per-implementation: the argument name
           is almost never the member name (quantise_battery_level takes 'pct'), and the floating
           and integer forms of one quantity differ in BOTH -- quantise_link_snr takes 'float snr'
           in one and 'int32_t snr10' in the other, while dequantise_link_snr returns 'float' or
           'int32_t'. Chasing that with one attribute at a time would have meant a new attribute
           per field; declaring the signature says it once. The template still owns `static`, the
           braces and every guard. */
        const impls = {};
        const collectImpls = (node, scope) => {
            for (const im of kids(node, 'impl')) {
                const lang = attr(im, 'lang');
                const hook = attr(im, 'hook');
                const when = attr(im, 'when', '');
                const key = `${lang}/${hook}${when ? '/' + when : ''}`;
                if (key in scope) throw new Error(`field '${name}': two <impl> for ${key}`);
                scope[key] = { body: textOf(im), ret: attr(im, 'ret', ''), arg: attr(im, 'arg', '') };
            }
        };
        collectImpls(f, impls);

        const quantities = kids(f, 'quantity').map((q) => {
            const qi = {};
            collectImpls(q, qi);
            return {
                name: attr(q, 'name'),
                type: attr(q, 'type'),
                param: attr(q, 'param'),
                /* the stem of the quantise_/dequantise_ function names, which is not always the
                   member name: datetime_secs is quantised by quantise_datetime, and
                   battery_charging by quantise_battery_state */
                stem: attr(q, 'stem', attr(q, 'name')),
                /* HOW THIS QUANTITY IS SPELLED where the scalar rule cannot derive it. A scalar
                   is "uint16_t x" in storage, "uint16_t x" as an argument and "uint16_t *x" on
                   the way out, so the generator builds all three from type and param. An ARRAY
                   is none of those: its storage carries an extent, its argument is const and
                   decays, and its out-parameter takes no star. Rather than teach the template
                   about arrays -- a sixth construct, which this design does not allow -- the
                   specification states the three spellings and the generator uses them as
                   given. Silence keeps the scalar forms. */
                member: attr(q, 'member', ''),
                arg: attr(q, 'arg', ''),
                arg_out: attr(q, 'arg_out', ''),
                impls: qi,
                /* A check's error usually belongs to the field that declares it, but a
                   composite validates its PARTS' ranges and raises the PARTS' errors -- so it
                   has checks in its setter and no error codes of its own. `field` names whose
                   error it is; without it, this field's. */
                /* HOW THIS QUANTITY MOVES, for the cases the scalar rule cannot derive. A scalar
                   is copied with =; an array is not, and a presence mask is masked on the way in.
                   Four movements, named for WHERE the field's data lives: a local under ORDERED,
                   the context's own member otherwise. Silence keeps the scalar forms. */
                store_local: textOrNone(kid(q, 'store_local')),
                store_member: textOrNone(kid(q, 'store_member')),
                fetch_local: textOrNone(kid(q, 'fetch_local')),
                fetch_member: textOrNone(kid(q, 'fetch_member')),
                /* A check is usually one expression and one error, but it can be a loop over an
                   array's slots, so the test may be given verbatim. The ERROR is still declared
                   and registered either way -- only the test it fires on is the spec's text. */
                checks: kids(q, 'check').map((c) => ({ expr: attr(c, 'expr', ''), error: attr(c, 'error'), text: attr(c, 'text'), field: attr(c, 'field', ''), test: textOrNone(c) })),
            };
        });

        const constants = kid(f, 'constants');
        return {
            name,
            id: attr(f, 'id'),
            bits: attr(f, 'bits'),
            parts: attr(f, 'parts', '').split(/\s+/).filter(Boolean),
            /* A COMPOSITE THAT CALLS ITS PARTS' OPERATIONS rather than formatting their values
               itself. Wind, environment, rain and radiation each pack and print their parts'
               quantities directly, so a part's operations are needed only when that part is
               enabled. Air quality's composite delegates -- its pack calls pack_air_quality_pm
               and so on for all five operations -- so its parts' operations must exist whenever
               the COMPOSITE is enabled, which is the tier-1 guard, not the part's own. */
            delegates: attr(f, 'delegates', '') === 'yes',
            /* A FIELD THE GENERATOR DESCRIBES BUT DOES NOT WRITE. IMAGE is a field in every
               sense the protocol cares about -- an identifier, a presence slot, an enum member,
               an ops entry -- but its code differs from every other field in structure rather
               than spelling (separate encode and decode storage, three scratch types, two
               library-wide scratch macros, a compression library, an extra decode parameter), so
               it is hand-written in iotdata_fields_image.h. Saying so here keeps it in the
               registry and in the identifier-collision check, which is the whole reason it has a
               specification document at all. */
            code: attr(f, 'code', 'yes') !== 'no',
            /* IMAGE's storage differs by direction -- a const pointer when encoding, the 254
               bytes themselves when decoding -- so it contributes MEM_IMAGE_ENCODE and
               MEM_IMAGE_DECODE where every other field contributes one MEM. The master lists
               are the only place that distinction shows, which is why it is a flag and not a
               second storage mechanism. */
            mem: attr(f, 'mem', 'single'),
            notes,
            impls,
            quantities,
            constants: constants ? kids(constants, 'constant').map((c) => ({ name: attr(c, 'name'), value: attr(c, 'value'), when: attr(c, 'when', ''), early: attr(c, 'early', '') === 'yes', lead: attr(c, 'lead', '') })) : [],
        };
    });
}

/* ---------------------------------------------------------------------------
 * The pump
 * -------------------------------------------------------------------------*/

const TAG = /\{\{(#each|#if|#field|\/each|\/if|\/field|impl|ret|arg|doc|field)?\s*([A-Za-z_][\w.]*)?\s*\}\}/;

/* Find the body of a block that starts at `from`, honouring nesting of the same
   kind, and return [body, indexAfterCloser]. */
function blockBody(tmpl, from, kind) {
    const open = `{{#${kind}`;
    const close = `{{/${kind}}}`;
    let depth = 1;
    let i = from;
    while (i < tmpl.length) {
        const no = tmpl.indexOf(open, i);
        const nc = tmpl.indexOf(close, i);
        if (nc === -1) throw new Error(`template: {{#${kind}}} is never closed`);
        if (no !== -1 && no < nc) {
            depth++;
            i = no + open.length;
            continue;
        }
        depth--;
        if (depth === 0) return [tmpl.slice(from, nc), nc + close.length];
        i = nc + close.length;
    }
    throw new Error(`template: {{#${kind}}} is never closed`);
}

function render(tmpl, scope, lang, fieldBlock) {
    if (fieldBlock) scope = { ...scope, fieldBlock };
    let out = '';
    let i = 0;

    for (;;) {
        const rest = tmpl.slice(i);
        const m = TAG.exec(rest);
        if (!m) {
            out += rest;
            return out;
        }
        out += rest.slice(0, m.index);
        const absAfter = i + m.index + m[0].length;
        const [, kindRaw, arg] = m;
        const kind = kindRaw || '';

        if (kind === '#field') {
            /* The definition site emits nothing: it only records the body. */
            const [body, after] = blockBody(tmpl, absAfter, 'field');
            if (scope.fieldBlock && scope.fieldBlock.body !== undefined) throw new Error('template: {{#field}} defined more than once');
            scope.fieldBlock.body = body;
            i = after;
            continue;
        }
        if (kind === 'field') {
            if (!scope.fieldBlock || scope.fieldBlock.body === undefined) throw new Error(`template: {{field ${arg}}} appears before {{#field}} defines the block`);
            const f = scope.fieldBlock.byName[arg];
            if (!f) throw new Error(`template: {{field ${arg}}} but the specification has no field '${arg}'`);
            scope.fieldBlock.placed[arg] = (scope.fieldBlock.placed[arg] || 0) + 1;
            scope.fieldBlock.order.push(arg);
            out += render(scope.fieldBlock.body, f, lang, scope.fieldBlock);
            i = absAfter;
            continue;
        }
        if (kind === '#each') {
            const [body, after] = blockBody(tmpl, absAfter, 'each');
            const items = scope.lists && scope.lists[arg];
            if (!items) throw new Error(`template: {{#each ${arg}}} but the current scope has no '${arg}' list`);
            /* WHERE AN ITEM SITS IN ITS LIST is data, not a new construct: a continued macro wants
               a backslash on every line but the last, and the only honest way to say that with
               presence tests alone is to let an item know it is the last one. */
            items.forEach((it, n) => {
                const pos = { first: n === 0, last: n === items.length - 1, notfirst: n > 0, notlast: n < items.length - 1 };
                out += render(body, { ...it, flags: { ...it.flags, ...pos } }, lang, scope.fieldBlock);
            });
            i = after;
            continue;
        }
        if (kind === '#if') {
            const [body, after] = blockBody(tmpl, absAfter, 'if');
            if (scope.flags && scope.flags[arg]) out += render(body, scope, lang, scope.fieldBlock);
            i = after;
            continue;
        }
        if (kind === 'impl' || kind === 'ret' || kind === 'arg') {
            const key = `${lang}/${arg.replace(/\./g, '/')}`;
            if (!(key in scope.impls)) throw new Error(`template: {{${kind} ${arg}}} but the specification has no <impl lang="${lang}" hook="${arg}"> here`);
            const im = scope.impls[key];
            if (kind === 'impl') out += im.body;
            else {
                /* 'quantise.floating' falls back to the default for 'quantise': which build an
                   implementation is for does not change what its signature means. */
                const hook = arg.split('.')[0];
                out += (kind === 'ret' ? im.ret : im.arg) || scope.defaults[`${kind}.${hook}`] || '';
            }
            i = absAfter;
            continue;
        }
        if (kind === 'doc') {
            if (!(arg in scope.notes)) throw new Error(`template: {{doc ${arg}}} but the specification has no <note for="${arg}"> here`);
            out += scope.notes[arg];
            i = absAfter;
            continue;
        }
        if (kind === '/each' || kind === '/if') throw new Error(`template: stray {{${kind}}}`);

        if (!(arg in scope.values)) throw new Error(`template: {{${arg}}} is not a value in the current scope`);
        out += scope.values[arg];
        i = absAfter;
    }
}

/* Wrap the spec objects in the shape render() walks: values, lists, flags. */
/* The flags are named presence tests and that is ALL they are -- there is no {{#unless}} and no
   negation in the template language, so where a template needs "the other case" the flag for it is
   named here (checked/unchecked). Keeping the asking side dumb is the point: a template can only
   test what the specification has actually said. */
function fieldScope(f, lang) {
    const hasImpl = (impls, hook) => `${lang}/${hook}` in impls || Object.keys(impls).some((k) => k.startsWith(`${lang}/${hook}/`));
    const quantityScope = (q) => ({
        values: {
            'q.name': q.name,
            'q.type': q.type,
            'q.param': q.param,
            'q.stem': q.stem,
            'q.member': q.member || `${q.type} ${q.name};`,
            'q.arg': q.arg || `${q.type} ${q.param}`,
            'q.arg_out': q.arg_out || `${q.type} *${q.param}`,
            /* the four movements, each falling back to the scalar form */
            'q.store_local': q.store_local || `    v.${q.name} = ${q.param};\n`,
            'q.store_member': q.store_member || `    enc->${f.name}.${q.name} = ${q.param};\n`,
            'q.fetch_local': q.fetch_local || `    if (${q.param})\n        *${q.param} = v.${q.name};\n`,
            'q.fetch_member': q.fetch_member || `    if (${q.param})\n        *${q.param} = dec->${f.name}.${q.name};\n`,
            NAME: f.name.toUpperCase(),
            name: f.name,
        },
        lists: { check: q.checks.map((c) => checkScope(f, c)) },
        /* A quantity need not be quantised at all: FLAGS and BITS32 go on the wire as they stand,
           so pack reads the member directly and there is no quantise_ function to emit. */
        flags: {
            quantised: hasImpl(q.impls, 'quantise'),
            unquantised: !hasImpl(q.impls, 'quantise'),
            /* one arithmetic for both builds, or one per build: the two forms of a quantity are
               not the same expression at two widths -- quantise_link_snr takes float snr in one
               and int32_t snr10 in the other -- so they are separate bodies under a #if/#else */
            quantised_plain: `${lang}/quantise` in q.impls,
            quantised_by_build: `${lang}/quantise/floating` in q.impls,
        },
        impls: q.impls,
        /* What a signature means when the specification stays silent: quantise takes the member
           and yields the wire word; dequantise takes the wire word and yields the member. */
        defaults: {
            'ret.quantise': 'uint32_t',
            'arg.quantise': `${q.type} ${q.name}`,
            'ret.dequantise': q.type,
            'arg.dequantise': 'uint32_t raw',
        },
        notes: f.notes,
    });
    const checkScope = (ff, c) => ({
        values: {
            'k.expr': c.expr,
            'k.error': c.error,
            'k.text': c.text,
            /* the whole identifier, so a borrowed error names the field it belongs to */
            'k.ident': `IOTDATA_ERR_${(c.field || ff.name).toUpperCase()}_${c.error}`,
            /* the test this error fires on: one expression, or whatever the specification
               wrote where an expression will not do (a loop across an array's slots) */
            'k.test': c.test || `    if (${c.expr})\n        return IOTDATA_ERR_${(c.field || ff.name).toUpperCase()}_${c.error};\n`,
            NAME: ff.name.toUpperCase(),
            name: ff.name,
        },
        lists: {},
        flags: {},
        impls: {},
        defaults: {},
        notes: ff.notes,
    });
    const allChecks = f.quantities.flatMap((q) => q.checks);
    /* Only the checks whose error is this field's OWN appear in its error macros. */
    const ownChecks = allChecks.filter((c) => !c.field);
    return {
        values: {
            NAME: f.name.toUpperCase(),
            name: f.name,
            id: f.id,
            bits: f.bits,
            /* registry columns: the whole-set view, which is the one thing no single field knows */
            'r.id': String(f.id).replace(/u$/i, ''),
            'r.kind': f.parts.length > 0 ? 'composite' : 'atomic',
            'r.bits': (f.constants.find((c) => c.name === 'BITS') || { value: '' }).value,
            'r.parts': f.parts.join(' '),
            'r.quantities': f.quantities.map((q) => q.name).join(' '),
            'r.params': f.quantities.map((q) => `${q.type} ${q.param}`).join('; '),
            /* A composite raises its PARTS' errors, so the name comes from the check's own
               field attribute where it has one -- exactly as k.ident resolves it for the C.
               Building it from f.name instead produced IOTDATA_ERR_RAIN_HIGH, which does not
               exist, and listed radiation's one error name twice. Deduplicated because this
               column is the SET of errors the field can raise. */
            'r.errors': [...new Set(allChecks.map((c) => `IOTDATA_ERR_${(c.field || f.name).toUpperCase()}_${c.error}`))].join(' '),
            'r.document': f.file ? require('path').basename(f.file) : '',
            /* The enable guard this field's storage sits behind, and the prefix its public
               functions need. A standalone field answers to its own name; a family member's
               storage must exist whenever its composite is enabled too, while its slot, its ops
               and its errors answer to the part alone. Values rather than template branches,
               because the only thing that varies is the condition itself. */
            enable_guard: [f.name, ...(f.family ? [f.family] : [])].map((x) => `defined(IOTDATA_ENABLE_${x.toUpperCase()})`).join(' || '),
            /* THE PUBLIC FUNCTIONS of a family member answer to the member alone: its setter
               resolves ITS slot and its decoder names ITS enum member, neither of which exists
               when only the composite is enabled. */
            member_guard: f.family ? `defined(IOTDATA_ENABLE_${f.name.toUpperCase()}) && ` : '',
            /* ITS INTERNAL OPERATIONS may not: where the composite delegates, pack_x and
               print_x are called from the composite's own pack and print, so they have to exist
               whenever EITHER is enabled -- which the tier-1 guard around them already says,
               leaving nothing for this to add. */
            ops_guard: f.family && !f.family_delegates ? `defined(IOTDATA_ENABLE_${f.name.toUpperCase()}) && ` : '',
            FAMILY: f.family ? f.family.toUpperCase() : '',
        },
        lists: {
            /* A composite's parts, in the order its storage nests them. Membership is declared
               HERE and nowhere else, which is why a family migrates as a unit: until the
               composite is in the specification, nothing knows its parts are parts. */
            part: f.parts.map((pn) => ({
                values: { 'p.name': pn, 'p.NAME': pn.toUpperCase(), NAME: f.name.toUpperCase(), name: f.name },
                lists: {},
                flags: {},
                impls: {},
                defaults: {},
                notes: f.notes,
            })),
            quantity: f.quantities.map(quantityScope),
            constant: [],
            constant_early: [],
            check: allChecks.map((c) => checkScope(f, c)),
            own_check: ownChecks.map((c) => checkScope(f, c)),
        },
        flags: {
            composite: f.parts.length > 0,
            atomic: f.parts.length === 0,
            checked: allChecks.length > 0,
            unchecked: ownChecks.length === 0,
            one_check: ownChecks.length === 1,
            many_checks: ownChecks.length > 1,
            single: f.quantities.length === 1,
            multi: f.quantities.length > 1,
            member: !!f.family,
            standalone: !f.family,
            /* A family shares ONE section banner, carried by whichever of its fields holds the
               note; the others emit none. Data, so the specification decides. */
            has_banner: 'field' in f.notes,
            /* the air quality family marks each member with a one-line layout summary; no
               other family does, so the flag asks whether this field declared one */
            has_subfield: 'subfield' in f.notes,
            /* FILE-SCOPE DATA THE WHOLE FAMILY SHARES. The air quality family indexes its slots
               through static tables -- names, labels, per-slot bits, resolutions and ceilings --
               which belong to no single function and so fit none of the per-function hooks. They
               live in the specification beside the constants they index, and are emitted once,
               after the family banner and before the first field's own code. */
            has_tables: hasImpl(f.impls, 'tables'),
            mem_single: f.mem !== 'split',
            mem_split: f.mem === 'split',
        },
        impls: f.impls,
        defaults: {},
        notes: f.notes,
    };
}

/* The constant list is built here rather than in readSpec because its macro names are made from
   the field's own name, which is a template concern expressed as data. */
function addConstants(scope, f) {
    /* A CONSTANT CAN DEPEND ON THE BUILD: LINK's signal-to-noise range is stated in volts in a
       floating build and in hundredths in an integer one, so those three constants sit inside a
       #if/#else that the plain ones around them are outside of. The generator works out where
       each RUN of equally-conditioned constants opens and closes, because a template with only
       presence tests cannot look at its neighbours -- and a run is the right unit, since the two
       arms want ONE #if/#else/#endif around them rather than one per constant. */
    /* A CONSTANT MARKED EARLY comes out BEFORE the storage declaration. Two fields want this
       for different reasons: an array member's extent IS a constant, so
       `uint16_t aq_pm[IOTDATA_AIR_QUALITY_PM_COUNT];` cannot be declared before COUNT exists;
       and POSITION simply groups its offsets and ranges above its storage. The attribute says
       WHERE, not why, which is the only thing both have in common. Everything else follows the
       typedef, so only the handful that ask for it move. */
    const buildConstant = (cs) => (c, n) => {
        const prev = n > 0 ? cs[n - 1].when : null;
        const next = n < cs.length - 1 ? cs[n + 1].when : null;
        /* A CONDITIONAL REGION is a maximal run of consecutive conditioned constants: the first
           opens the #if, a change of condition inside it is the #else, and the last closes the
           #endif. Two arms give #if/#else/#endif; ONE arm gives #if/#endif, which a floating-only
           constant needs -- IOTDATA_WIND_DIRECTION_SCALE exists only where there is a float, and
           has no integer counterpart to be the #else of. */
        const opens = c.when !== '' && prev !== c.when && (prev === null || prev === '');
        const closes = c.when !== '' && (next === null || next === '');
        const elses = c.when !== '' && prev !== null && prev !== '' && prev !== c.when;
        return {
            values: { 'c.name': c.name, 'c.value': c.value, 'c.lead': c.lead, NAME: f.name.toUpperCase(), name: f.name },
            lists: {},
            /* a heading above a block of related constants: the gas slots' widths, their
               resolutions and their ceilings each get one in the hand-written original */
            flags: { opens_cond: opens, emit_else: elses, closes_cond: closes, has_lead: c.lead !== '', [`when_${c.when || 'none'}`]: true },
            impls: {},
            defaults: {},
            notes: f.notes,
        };
    };
    const csEarly = f.constants.filter((c) => c.early);
    const cs = f.constants.filter((c) => !c.early);
    scope.lists.constant_early = csEarly.map(buildConstant(csEarly));
    scope.lists.constant = cs.map(buildConstant(cs));
}

/* ---------------------------------------------------------------------------
 * Main
 * -------------------------------------------------------------------------*/

function main(argv) {
    const here = __dirname;
    const opt = {
        spec: path.join(here, '..', 'specification'),
        pattern: '^iotdata-fields.*\\.xml$',
        registry: 'iotdata_fields.csv',
        templates: path.join(here, 'templates'),
        out: path.join(here, '..'),
        stdout: null,
    };
    for (let i = 0; i < argv.length; i += 2) {
        const k = argv[i].replace(/^--/, '');
        if (!(k in opt)) throw new Error(`unknown option '${argv[i]}'`);
        opt[k] = argv[i + 1];
    }

    /* Every matching document in the specification directory, so a vendor adds fields by dropping
       a file in rather than by editing anyone else's. Sorted for reproducible error messages. */
    const st = fs.statSync(opt.spec);
    const files = st.isDirectory()
        ? fs.readdirSync(opt.spec)
              .filter((n) => new RegExp(opt.pattern).test(n))
              .sort()
              .map((n) => path.join(opt.spec, n))
        : [opt.spec];
    if (files.length === 0) throw new Error(`${opt.spec}: no specification documents match /${opt.pattern}/`);
    const fields = readSpecs(files);

    /* The registry is generated BY DEFAULT: it is the whole-set view, and a view that is only
       produced when someone remembers to ask for it is a view that goes stale. --registry takes a
       path, or 'none' to skip it. Like the C outputs it comes from a template, so its columns are
       changed without touching this program. */
    /* ONE FILE, TWO PHASES. The declarations and the definitions used to be a .h and a .c, with
       iotdata.c doing `#include "iotdata_fields.c"` -- a .c file included into another .c file,
       which is a thing no reader expects to see twice. They are one header now, on the pattern
       iotdata_tlv.h and iotdata_fields_image.h already follow: the declarations always, and the
       definitions only where IOTDATA_FIELDS_IMPLEMENTATION says so. Two templates still, because
       the output is [all declarations][all definitions] while a merged template would interleave
       them field by field. */
    const outputs = [
        {
            out: 'iotdata_fields.h',
            lang: 'c',
            parts: [
                { tmpl: 'fields.c.h.tmpl' },
                { tmpl: 'fields.c.c.tmpl', select: 'IOTDATA_FIELDS_IMPLEMENTATION', guard: 'IOTDATA_FIELDS_H_DEFINITIONS' },
            ],
        },
    ];
    if (opt.registry !== 'none')
        outputs.push({ out: opt.registry, lang: 'c', parts: [{ tmpl: 'fields.registry.csv.tmpl' }] });

    const written = [];
    for (const { out: outName, lang, parts } of outputs) {
      const rendered = [];
      for (const part of parts) {
        const tmplName = part.tmpl;
        const tmpl = fs.readFileSync(path.join(opt.templates, tmplName), 'utf8');
        /* Scopes are built per language: whether a quantity is quantised at all depends on which
           <impl> elements exist for THIS language, so the same field can be shaped differently in
           two outputs. */
        const scopes = fields.map((f, n) => {
            const sc = fieldScope(f, lang);
            addConstants(sc, f);
            return sc;
        });
        const byName = {};
        fields.forEach((f, n) => (byName[f.name] = scopes[n]));
        const fieldBlock = { body: undefined, byName, placed: {}, order: [] };
        /* THE FILE-LEVEL 'field' LIST IS IN IDENTIFIER ORDER, not document order, because the only
           output that iterates it is the registry and a registry reads by identifier -- blocks and
           gaps are then visible at a glance. The C templates do not iterate: they place by name,
           so for them this order is irrelevant. */
        const inIdOrder = fields
            .map((f, n) => ({ f, sc: scopes[n] }))
            .sort((a2, b2) => Number(String(a2.f.id).replace(/u$/i, '')) - Number(String(b2.f.id).replace(/u$/i, '')))
            .map((e) => e.sc);
        const text = render(tmpl, { values: {}, lists: { field: inIdOrder }, flags: {}, impls: {}, notes: {}, defaults: {}, fieldBlock }, lang);
        /* A field in the specification that the template never places would vanish from the
           output silently, which is the one generator mistake that a byte-identical diff cannot
           distinguish from "not migrated yet". */
        /* ...but only for a template that places fields at all. A template which iterates them
           instead (the registry) has every field by construction and nothing to forget. */
        if (fieldBlock.body !== undefined) {
            for (const f of fields) {
                const n = fieldBlock.placed[f.name] || 0;
                /* a field whose code is hand-written is not expected to be placed -- but it must
                   not be placed either, since the template cannot generate what it does not
                   describe */
                if (!f.code) {
                    if (n > 0) throw new Error(`${tmplName}: field '${f.name}' says code="no" but the template places it anyway`);
                    continue;
                }
                if (n === 0) throw new Error(`${tmplName}: field '${f.name}' is in the specification but the template never places it ({{field ${f.name}}})`);
                if (n > 1) throw new Error(`${tmplName}: field '${f.name}' is placed ${n} times`);
            }
            /* AND A COMPOSITE AFTER ITS PARTS. pack_environment calls pack_temperature, so a
               template placing the composite first would emit a call to something not yet
               declared -- which fails to compile with a message about the C rather than about
               the template. Saying it here names the template and the two fields. This is the
               ONLY relationship between fields the generator knows, and composition is the only
               one that cannot be removed by writing a field out in full. */
            for (const f of fields)
                for (const dep of f.parts)
                    if (fieldBlock.order.indexOf(dep) > fieldBlock.order.indexOf(f.name))
                        throw new Error(`${tmplName}: '${f.name}' is placed before '${dep}', which it depends on -- {{field ${dep}}} must come first`);
        }
        rendered.push({ part, text });
      }
      /* the phases, wrapped as the hand-written two-phase headers are */
      let text = '';
      for (const { part, text: body } of rendered) {
          if (part.select) {
              text += `\n#if defined(${part.select}) && !defined(${part.guard})\n#define ${part.guard}\n\n`;
              text += body.replace(/\n+$/, '\n');
              text += `\n#endif /* ${part.select} */\n`;
          } else {
              text += body;
          }
      }
      if (opt.stdout === outName) {
          process.stdout.write(text);
      } else {
          fs.writeFileSync(path.join(opt.out, outName), text);
          written.push(`${outName} (${text.split('\n').length} lines)`);
      }
    }
    if (written.length)
        console.log(`generated ${written.join(', ')}\n  from ${fields.length} field(s) in ${files.length} document(s): ${files.map((f) => path.basename(f)).join(', ')}`);
}

try {
    main(process.argv.slice(2));
} catch (e) {
    console.error(`iotdata-generator-fields: ${e.message}`);
    process.exit(1);
}
