#ifndef SEQUENCE_H
#define SEQUENCE_H

enum seq_id {
	SEQ_SOUTH,
	SEQ_WEST,
	SEQ_EAST,
	SEQ_NORTH,
};

struct sequence {
	unsigned short width, height;
	unsigned short count;
	unsigned short seq_id;
	struct coord { unsigned short x, y; } coord[];
};

struct seq_group {
	unsigned count;
	struct sequence *seq[];
};

/* TODO: move this */
#define CHARACTER_TILE_M 3
#define CHARACTER_TILE_N 4

struct sheet;

struct sequence *sequence_new(enum seq_id id,
	int width, int height, unsigned count);
void sequence_free(struct sequence *seq);
struct seq_group *seq_group_new(unsigned count);
struct seq_group *seq_group_resize(struct seq_group *old, unsigned count);
void seq_group_free(struct seq_group *sg);
struct seq_group *sequence_for_character(int x, int y, int width, int height);
struct seq_group *sequence_group_load(const char *filename);

#endif
