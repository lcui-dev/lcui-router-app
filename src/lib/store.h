struct store {
    char head[32];
    char version[32];
    char message[256];
};

extern void store_init(struct store *s);

extern int store_load(struct store *s);

extern int store_save(struct store *s);
