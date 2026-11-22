/*
 * IoT Sensor Telemetry Protocol
 * Copyright(C) 2026 Matthew Gream (https://libiotdata.org)
 *
 * iotdata-generator-functions-xml.js — a deliberately small XML reader
 *
 * WHY THIS EXISTS RATHER THAN A DEPENDENCY. Nothing else in this tree pulls an
 * XML library, and the specification is a document we write and we read: it has
 * no namespaces, no DTD, no entity definitions, no processing instructions
 * beyond the declaration. A reader for that is a page of code, and a page of
 * code is also what a generator in another language will need when it comes to
 * read the same specification. A dependency would have to be found, pinned and
 * audited once per language instead.
 *
 * WHAT IT SUPPORTS: elements, attributes (single or double quoted), self-closing
 * elements, comments, CDATA, text, and the five predefined entities. CDATA is
 * the point of the whole exercise -- the specification carries bodies of C
 * verbatim, and CDATA is the one construct that holds arbitrary text without
 * escaping a single character of it.
 *
 * WHAT IT REFUSES: anything else, loudly and with a line number. A specification
 * that does not parse should say which line is wrong, because the alternative is
 * a generator that emits subtly wrong C.
 */

'use strict';

const ENTITIES = { lt: '<', gt: '>', amp: '&', quot: '"', apos: "'" };

function decodeEntities(s, where) {
    return s.replace(/&(#x?[0-9a-fA-F]+|[a-zA-Z]+);/g, (whole, body) => {
        if (body[0] === '#') {
            const code = body[1] === 'x' || body[1] === 'X' ? parseInt(body.slice(2), 16) : parseInt(body.slice(1), 10);
            if (!Number.isFinite(code)) throw new Error(`${where}: bad numeric entity ${whole}`);
            return String.fromCodePoint(code);
        }
        if (!(body in ENTITIES)) throw new Error(`${where}: unknown entity ${whole} (only the five predefined ones are supported)`);
        return ENTITIES[body];
    });
}

/*
 * Parse a document into a tree of
 *   { tag, attrs: {name: value}, children: [node | string], line }
 * Text children are plain strings; CDATA arrives as a plain string too, because
 * by the time it is parsed the distinction has done its job.
 */
function parseXml(text, filename = '<spec>') {
    let i = 0;
    const lineOf = (pos) => text.slice(0, pos).split('\n').length;
    const at = (pos) => `${filename}:${lineOf(pos)}`;

    // the declaration, if present, carries nothing we need
    const decl = /^\s*<\?xml[^?]*\?>/.exec(text);
    if (decl) i = decl[0].length;

    const stack = [];
    let root = null;

    const addChild = (node) => {
        if (stack.length === 0) {
            if (root !== null) throw new Error(`${at(i)}: a second root element (${node.tag}) -- a document has exactly one`);
            root = node;
        } else {
            stack[stack.length - 1].children.push(node);
        }
    };

    while (i < text.length) {
        const lt = text.indexOf('<', i);

        // text between elements
        if (lt === -1 || lt > i) {
            const raw = text.slice(i, lt === -1 ? text.length : lt);
            if (stack.length > 0) stack[stack.length - 1].children.push(decodeEntities(raw, at(i)));
            else if (raw.trim() !== '') throw new Error(`${at(i)}: text outside the root element`);
            if (lt === -1) break;
            i = lt;
        }

        if (text.startsWith('<!--', i)) {
            const end = text.indexOf('-->', i + 4);
            if (end === -1) throw new Error(`${at(i)}: unterminated comment`);
            i = end + 3;
            continue;
        }

        if (text.startsWith('<![CDATA[', i)) {
            const end = text.indexOf(']]>', i + 9);
            if (end === -1) throw new Error(`${at(i)}: unterminated CDATA`);
            if (stack.length === 0) throw new Error(`${at(i)}: CDATA outside any element`);
            // verbatim: not entity-decoded, not whitespace-touched. That is the whole point.
            stack[stack.length - 1].children.push(text.slice(i + 9, end));
            i = end + 3;
            continue;
        }

        if (text.startsWith('</', i)) {
            const end = text.indexOf('>', i);
            if (end === -1) throw new Error(`${at(i)}: unterminated closing tag`);
            const name = text.slice(i + 2, end).trim();
            const open = stack.pop();
            if (!open) throw new Error(`${at(i)}: closing </${name}> with nothing open`);
            if (open.tag !== name) throw new Error(`${at(i)}: closing </${name}> but <${open.tag}> is open (opened at line ${open.line})`);
            i = end + 1;
            continue;
        }

        if (text.startsWith('<?', i) || text.startsWith('<!', i)) throw new Error(`${at(i)}: unsupported construct -- this reader handles elements, attributes, comments and CDATA only`);

        // an opening (or self-closing) tag
        const m = /^<([A-Za-z_][\w.-]*)/.exec(text.slice(i));
        if (!m) throw new Error(`${at(i)}: expected an element name after '<'`);
        const node = { tag: m[1], attrs: {}, children: [], line: lineOf(i) };
        let j = i + m[0].length;

        for (;;) {
            while (j < text.length && /\s/.test(text[j])) j++;
            if (text.startsWith('/>', j)) {
                addChild(node);
                j += 2;
                break;
            }
            if (text[j] === '>') {
                addChild(node);
                stack.push(node);
                j += 1;
                break;
            }
            const am = /^([A-Za-z_][\w.-]*)\s*=\s*("([^"]*)"|'([^']*)')/.exec(text.slice(j));
            if (!am) throw new Error(`${at(j)}: expected an attribute, '>' or '/>' inside <${node.tag}>`);
            const name = am[1];
            if (name in node.attrs) throw new Error(`${at(j)}: attribute '${name}' given twice on <${node.tag}>`);
            node.attrs[name] = decodeEntities(am[3] !== undefined ? am[3] : am[4], at(j));
            j += am[0].length;
        }
        i = j;
    }

    if (stack.length > 0) {
        const open = stack[stack.length - 1];
        throw new Error(`${filename}: <${open.tag}> opened at line ${open.line} is never closed`);
    }
    if (root === null) throw new Error(`${filename}: no root element`);
    return root;
}

/* ---------------------------------------------------------------------------
 * Walking helpers
 *
 * Small on purpose: the generator asks for a child, a list of children, an
 * attribute or the text of an element, and nothing else. An attribute that is
 * required and missing is an error naming the element and the line, because a
 * specification is edited by hand and a silent default is how a field ends up
 * with the wrong number of bits.
 * -------------------------------------------------------------------------*/

const kids = (node, tag) => node.children.filter((c) => typeof c === 'object' && c.tag === tag);

function kid(node, tag, required = false) {
    const all = kids(node, tag);
    if (all.length > 1) throw new Error(`<${node.tag}> at line ${node.line}: more than one <${tag}>`);
    if (all.length === 0 && required) throw new Error(`<${node.tag}> at line ${node.line}: missing required <${tag}>`);
    return all[0] || null;
}

function attr(node, name, fallback = undefined) {
    if (name in node.attrs) return node.attrs[name];
    if (fallback !== undefined) return fallback;
    throw new Error(`<${node.tag}> at line ${node.line}: missing required attribute '${name}'`);
}

const hasAttr = (node, name) => name in node.attrs;

/* The concatenated text of an element, verbatim. Used for CDATA bodies, so it
   must not trim or collapse anything the author wrote. */
const textOf = (node) => node.children.filter((c) => typeof c === 'string').join('');

module.exports = { parseXml, kid, kids, attr, hasAttr, textOf };
