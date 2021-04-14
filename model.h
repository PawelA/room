struct mtl {
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shininess;
};

struct model {
	int vert_cnt;
	int face_cnt;

	unsigned vert_array_id;

	unsigned coord_buff_id;
	unsigned normal_buff_id;
	unsigned element_buff_id;

	struct mtl mtl;
};

int model_load(struct model *model, char *filename);

