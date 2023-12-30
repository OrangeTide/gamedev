/* Two types of animation:
 * autoloop - frame counter is used to index a loop of frames.
 * manual - animation is outside of the scene graph, updated by user functions.
 */

struct coord {
	short x, y;
};

struct bbx {
	unsigned short x, y, w, h;
};

/* subsurface of a larger surface */
struct image_data {
	struct bbx surf_bbx;
	struct coord origin;
};

/* animation sequence */
struct animation {
	unsigned num_frames; /* use 1 frame to represent 'manual' type */
	struct image_data *frames[];
};

struct map {
	int x, y, width, height; /* offsets in world coordinates */
	struct animation **data; /* NxM tiles */
};

struct sprite {
	int x, y, width, height; /* offsets in world coordinates */
	struct animation *data; /* a single frame */
};

struct world {
	unsigned num_maps;
	struct map *maps;
	unsigned num_sprites;
	struct sprite *sprites; /* TODO: use a faster lookup mechanism */
	struct surface *surf; /* one sheet per world. TODO: multiple sheets. */
};
