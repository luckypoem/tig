#define VERSION	"tig-0.6.git"

/* Revision graph */

#define REVGRAPH_INIT	'I'
#define REVGRAPH_MERGE	'M'
#define REVGRAPH_BRANCH	'+'
#define REVGRAPH_COMMIT	'*'
#define REVGRAPH_LINE	'|'

	"git ls-remote $(git rev-parse --git-dir) 2>/dev/null"
	unsigned int remote:1;	/* Is it a remote ref? */
	if (bufsize < SIZEOF_STR)
		buf[bufsize] = 0;

static bool opt_rev_graph		= FALSE;
			string_copy(opt_cmd, "git log --pretty=raw");
LINE(MAIN_REMOTE,  "",			COLOR_YELLOW,	COLOR_DEFAULT,	A_BOLD), \
	struct keybinding *keybinding;
	keybinding = calloc(1, sizeof(*keybinding));
	char *sep = "";
	VIEW_(HELP,  "help",  &pager_ops, ""),
	VIEW_(PAGER, "pager", &pager_ops, ""),
		view->offset += scroll_steps;
		assert(0 <= view->offset && view->offset < view->lines);
		/* When running random commands, initially show the
		 * command in the title. However, it maybe later be
		 * overwritten if a commit line is selected. */
		string_copy(view->ref, view->cmd);
		char path[SIZEOF_STR];
			opt_path[0] = path[0] = 0;
		else if (sq_quote(path, 0, opt_path) >= sizeof(path))
			return FALSE;
		if (!string_format(view->cmd, format, id, path))

		/* Put the current ref_* value to the view title ref
		 * member. This is needed by the blob view. Most other
		 * views sets it automatically after loading because the
		 * first line is a commit line. */
		string_copy(view->ref, id);
		/* Since revision graph visualization requires knowledge
		 * about the parent commit, it causes a further one-off
		 * needed to be redrawn for incremental updates. */
		if (redraw_from > 0 && opt_rev_graph)
			redraw_from--;

	view->ops->read(view, NULL);
static struct line *
add_line_text(struct view *view, char *data, enum line_type type)
{
	struct line *line = &view->line[view->lines];

	if (!data)
		return NULL;

	line->data = strdup(data);
	if (!line->data)
		return NULL;

	line->type = type;
	view->lines++;

	return line;
}

	add_line_text(view, "Quick reference for tig keybindings:", LINE_DEFAULT);
			add_line_text(view, "", LINE_DEFAULT);
			add_line_text(view, req_info[i].help, LINE_DEFAULT);
		if (!string_format(buf, "    %-25s %s", key, req_info[i].help))
		add_line_text(view, buf, LINE_DEFAULT);
	if (!string_format(refbuf, "git describe %s 2>/dev/null", commit_id))
		char *fmt = ref->tag    ? "%s[%s]" :
		            ref->remote ? "%s<%s>" : "%s%s";
	add_line_text(view, buf, LINE_PP_REFS);
	struct line *line;
	if (!data)
		return TRUE;
	line = add_line_text(view, data, get_line_type(data));
	if (!line)
		return FALSE;
		char *text = line->data + STRING_SIZE("commit ");
		if (view != VIEW(REQ_VIEW_PAGER))
			string_copy(view->ref, text);
		string_copy(ref_commit, text);
	size_t textlen = text ? strlen(text) : 0;
		if (!string_format(buf, "Directory path /%s", opt_path) ||
		    !realloc_lines(view, view->line_size + 1) ||
		    !add_line_text(view, buf, LINE_DEFAULT))
		if (*opt_path) {
			if (!string_format(buf, TREE_UP_FORMAT, view->ref) ||
			    !realloc_lines(view, view->line_size + 1) ||
			    !add_line_text(view, buf, LINE_TREE_DIR))
				return FALSE;
		}
	if (!add_line_text(view, text, type))
	enum open_flags flags;
		flags = OPEN_RELOAD;
		flags = display[0] == view ? OPEN_SPLIT : OPEN_DEFAULT;
	char *text = line->data + STRING_SIZE("100644 blob ");
	return add_line_text(view, line, LINE_DEFAULT);
 * Revision graph
	char title[128];		/* First line of the commit message. */
/* Size of rev graph with no  "padding" columns */
#define SIZEOF_REVITEMS	(SIZEOF_REVGRAPH - (SIZEOF_REVGRAPH / 2))

struct rev_graph {
	struct rev_graph *prev, *next, *parents;
	char rev[SIZEOF_REVITEMS][SIZEOF_REV];
	size_t size;
	struct commit *commit;
	size_t pos;
};

/* Parents of the commit being visualized. */
static struct rev_graph graph_parents[4];

/* The current stack of revisions on the graph. */
static struct rev_graph graph_stacks[4] = {
	{ &graph_stacks[3], &graph_stacks[1], &graph_parents[0] },
	{ &graph_stacks[0], &graph_stacks[2], &graph_parents[1] },
	{ &graph_stacks[1], &graph_stacks[3], &graph_parents[2] },
	{ &graph_stacks[2], &graph_stacks[0], &graph_parents[3] },
};

static inline bool
graph_parent_is_merge(struct rev_graph *graph)
{
	return graph->parents->size > 1;
}

static inline void
append_to_rev_graph(struct rev_graph *graph, chtype symbol)
{
	struct commit *commit = graph->commit;

	if (commit->graph_size < ARRAY_SIZE(commit->graph) - 1)
		commit->graph[commit->graph_size++] = symbol;
}

static void
done_rev_graph(struct rev_graph *graph)
{
	if (graph_parent_is_merge(graph) &&
	    graph->pos < graph->size - 1 &&
	    graph->next->size == graph->size + graph->parents->size - 1) {
		size_t i = graph->pos + graph->parents->size - 1;

		graph->commit->graph_size = i * 2;
		while (i < graph->next->size - 1) {
			append_to_rev_graph(graph, ' ');
			append_to_rev_graph(graph, '\\');
			i++;
		}
	}

	graph->size = graph->pos = 0;
	graph->commit = NULL;
	memset(graph->parents, 0, sizeof(*graph->parents));
}

static void
push_rev_graph(struct rev_graph *graph, char *parent)
{
	int i;

	/* "Collapse" duplicate parents lines.
	 *
	 * FIXME: This needs to also update update the drawn graph but
	 * for now it just serves as a method for pruning graph lines. */
	for (i = 0; i < graph->size; i++)
		if (!strncmp(graph->rev[i], parent, SIZEOF_REV))
			return;

	if (graph->size < SIZEOF_REVITEMS) {
		string_ncopy(graph->rev[graph->size++], parent, SIZEOF_REV);
	}
}

static chtype
get_rev_graph_symbol(struct rev_graph *graph)
{
	chtype symbol;

	if (graph->parents->size == 0)
		symbol = REVGRAPH_INIT;
	else if (graph_parent_is_merge(graph))
		symbol = REVGRAPH_MERGE;
	else if (graph->pos >= graph->size)
		symbol = REVGRAPH_BRANCH;
	else
		symbol = REVGRAPH_COMMIT;

	return symbol;
}

static void
draw_rev_graph(struct rev_graph *graph)
{
	struct rev_filler {
		chtype separator, line;
	};
	enum { DEFAULT, RSHARP, RDIAG, LDIAG };
	static struct rev_filler fillers[] = {
		{ ' ',	REVGRAPH_LINE },
		{ '`',	'.' },
		{ '\'',	' ' },
		{ '/',	' ' },
	};
	chtype symbol = get_rev_graph_symbol(graph);
	struct rev_filler *filler;
	size_t i;

	filler = &fillers[DEFAULT];

	for (i = 0; i < graph->pos; i++) {
		append_to_rev_graph(graph, filler->line);
		if (graph_parent_is_merge(graph->prev) &&
		    graph->prev->pos == i)
			filler = &fillers[RSHARP];

		append_to_rev_graph(graph, filler->separator);
	}

	/* Place the symbol for this revision. */
	append_to_rev_graph(graph, symbol);

	if (graph->prev->size > graph->size)
		filler = &fillers[RDIAG];
	else
		filler = &fillers[DEFAULT];

	i++;

	for (; i < graph->size; i++) {
		append_to_rev_graph(graph, filler->separator);
		append_to_rev_graph(graph, filler->line);
		if (graph_parent_is_merge(graph->prev) &&
		    i < graph->prev->pos + graph->parents->size)
			filler = &fillers[RSHARP];
		if (graph->prev->size > graph->size)
			filler = &fillers[LDIAG];
	}

	if (graph->prev->size > graph->size) {
		append_to_rev_graph(graph, filler->separator);
		if (filler->line != ' ')
			append_to_rev_graph(graph, filler->line);
	}
}

/* Prepare the next rev graph */
static void
prepare_rev_graph(struct rev_graph *graph)
{
	size_t i;

	/* First, traverse all lines of revisions up to the active one. */
	for (graph->pos = 0; graph->pos < graph->size; graph->pos++) {
		if (!strcmp(graph->rev[graph->pos], graph->commit->id))
			break;

		push_rev_graph(graph->next, graph->rev[graph->pos]);
	}

	/* Interleave the new revision parent(s). */
	for (i = 0; i < graph->parents->size; i++)
		push_rev_graph(graph->next, graph->parents->rev[i]);

	/* Lastly, put any remaining revisions. */
	for (i = graph->pos + 1; i < graph->size; i++)
		push_rev_graph(graph->next, graph->rev[i]);
}

static void
update_rev_graph(struct rev_graph *graph)
{
	/* If this is the finalizing update ... */
	if (graph->commit)
		prepare_rev_graph(graph);

	/* Graph visualization needs a one rev look-ahead,
	 * so the first update doesn't visualize anything. */
	if (!graph->prev->commit)
		return;

	draw_rev_graph(graph->prev);
	done_rev_graph(graph->prev->prev);
}


/*
 * Main view backend
 */

		waddch(view->win, ' ');
			else if (commit->refs[i]->remote)
				wattrset(view->win, get_line_attr(LINE_MAIN_REMOTE));
	static struct rev_graph *graph = graph_stacks;
	enum line_type type;
	if (!line) {
		update_rev_graph(graph);
		return TRUE;
	}

	type = get_line_type(line);

		graph->commit = commit;
		break;

	case LINE_PARENT:
		if (commit) {
			line += STRING_SIZE("parent ");
			push_rev_graph(graph->parents, line);
		}
		/* Parse author lines where the name may be empty:
		 *	author  <email@address.tld> 1138474660 +0100
		 */
		char *nameend = strchr(ident, '<');
		char *emailend = strchr(ident, '>');
		if (!commit || !nameend || !emailend)
		update_rev_graph(graph);
		graph = graph->next;
		*nameend = *emailend = 0;
		ident = chomp_string(ident);
		if (!*ident) {
			ident = chomp_string(nameend + 1);
			if (!*ident)
				ident = "Unknown";
		if (emailend[1] == ' ') {
			char *secs = emailend + 2;
			char *zone = strchr(secs, ' ');
			time_t time = (time_t) atol(secs);

		if (strncmp(line, "    ", 4))
			break;
		line += 4;
		/* Well, if the title starts with a whitespace character,
		 * try to be forgiving.  Otherwise we end up with no title. */
		while (isspace(*line))
			line++;
		if (*line == '\0')
		/* FIXME: More graceful handling of titles; append "..." to
		 * shortened titles, etc. */
		string_copy(commit->title, line);
	bool remote = FALSE;
	} else if (!strncmp(name, "refs/remotes/", STRING_SIZE("refs/remotes/"))) {
		remote = TRUE;
		namelen -= STRING_SIZE("refs/remotes/");
		name	+= STRING_SIZE("refs/remotes/");

	ref->remote = remote;
		/* wgetch() with nodelay() enabled returns ERR when there's no
		 * input. */
		if (key == ERR) {
			request = REQ_NONE;
			continue;
		}
