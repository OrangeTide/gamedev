#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sequence.h"
#include "logger.h"

struct sequence *
sequence_new(enum seq_id id, int width, int height, unsigned count)
{
	struct sequence *seq;

	seq = calloc(1, sizeof(seq) + count * sizeof(*seq->coord));
	if (!seq)
		return NULL;
	seq->count = count;
	seq->width = width;
	seq->height = height;
	seq->seq_id = id;
	return seq;
}

void
sequence_free(struct sequence *seq)
{
	free(seq);
}

/* creates a list of sequences */
struct seq_group *
seq_group_new(unsigned count)
{
	struct seq_group *sg;

	sg = calloc(1, sizeof(*sg) + count * sizeof(*sg->seq));
	sg->count = count;
	return sg;
}

struct seq_group *
seq_group_resize(struct seq_group *old, unsigned count)
{
	struct seq_group *new;
	unsigned old_count;

	if (!old)
		return seq_group_new(count);
	old_count = old->count;
	if (old_count == count)
		return old;
	new = realloc(old, sizeof(*new) + count * sizeof(*new->seq));
	if (!new)
		return old; // TODO: handle errors
	if (old_count < count)
		memset(new->seq + old_count, 0,
			sizeof(*new->seq) * (count - old_count));
	new->count = count;
	return new;
}

void
seq_group_free(struct seq_group *sg)
{
	unsigned i;

	if (!sg)
		return;
	for (i = 0; i < sg->count; i++) {
		sequence_free(sg->seq[i]);
		sg->seq[i] = NULL;
	}
	free(sg);
}

/* loads a 3x4 region of sprites into a sequence.
 * see CHARACTER_TILE_M and CHARACTER_TILE_N
 */
struct seq_group *
sequence_for_character(int x, int y, int width, int height)
{
	struct seq_group *sg;
	struct sequence *seq;
	unsigned i, j;

	sg = seq_group_new(CHARACTER_TILE_N);
	if (!sg)
		return NULL;

	for (i = 0; i < CHARACTER_TILE_N; i++) {
		seq = sequence_new(SEQ_NORTH + i, width, height,
			CHARACTER_TILE_M);
		if (!seq)
			goto fail;
		for (j = 0; j < CHARACTER_TILE_M; j++) {
			seq->coord[j].x = x + i * height;
			seq->coord[j].y = y + j * width;
		}
		sg->seq[i] = seq;
	}
	return sg;
fail:
	seq_group_free(sg);
	return NULL;
}

/* sequence-id frame-count widthxheight+x+y ... */

struct seq_group *
sequence_group_load(const char *filename)
{
	FILE *f;
	struct seq_group *sg;
	struct sequence *seq;
	unsigned seq_id;
	unsigned count;
	unsigned width, height, x, y;
	unsigned i;

	f = fopen(filename, "r");
	if (!f) {
		Error("%s:%s\n", filename, strerror(errno));
		return 0;
	}
	sg = seq_group_new(0);

	while (fscanf(f, " %u count=%u", &seq_id, &count) == 2) {
		if (!count)
			goto fail;
		for (i = 0; i < count; i++) {
			// Debug("item%d/%d:START\n", i, count);
			if (fscanf(f, " %ux%u+%u+%u", &width, &height, &x, &y) != 4)
				goto fail;
			if (i == 0) {
				seq = sequence_new(seq_id, width, height, count);
			} else if (seq->width != width || seq->height != height) {
				Error("%s:variable frame size not supported!\n",
					filename);
				goto fail;
			}
			seq->coord[i].x = x;
			seq->coord[i].y = y;
			// Debug("item%d/%d:DONE %ux%u+%u+%u\n", i, count, width, height, x, y);
		}
		fscanf(f, "\n");

		sg = seq_group_resize(sg, sg->count + 1);
		sg->seq[sg->count - 1] = seq;
		seq = NULL;
	}
	return sg;
fail:
	sequence_free(seq);
	seq_group_free(sg);
	fclose(f);
	Error("%s:unable to load sequence!\n", filename);
	return 0;
}
