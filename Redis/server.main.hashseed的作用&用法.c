
2.1.生成随机的hex的hash种子, 种子的作用是什么? 什么地方会用到?
    getRandomHexChars(hashseed,sizeof(hashseed));         /* 生成随机的hex的hash种子, 种子的作用是什么? 什么地方会用到? */
    dictSetHashFunctionSeed((uint8_t*)hashseed);
A:  a.生成全局的seed,并拷贝到dict_hash_function_seed中,
    b.直接用到的地方
    static uint8_t dict_hash_function_seed[16];

    void dictSetHashFunctionSeed(uint8_t *seed) {
        memcpy(dict_hash_function_seed,seed,sizeof(dict_hash_function_seed));   /** 保存seed */
    }

    uint8_t *dictGetHashFunctionSeed(void) {     /** 该方法没有发现直接使用的地方 */
        return dict_hash_function_seed;
    }

    /* The default hashing function uses SipHash implementation
     * in siphash.c. */

    uint64_t siphash(const uint8_t *in, const size_t inlen, const uint8_t *k);
    uint64_t siphash_nocase(const uint8_t *in, const size_t inlen, const uint8_t *k);

    uint64_t dictGenHashFunction(const void *key, int len) {       /** 引用较多  */
        return siphash(key,len,dict_hash_function_seed);
    }

    uint64_t dictGenCaseHashFunction(const unsigned char *buf, int len) {         /** 引用较多  */
        return siphash_nocase(buf,len,dict_hash_function_seed);
    }
    a.seed的生成原理:

util.c
    /* Generate the Redis "Run ID", a SHA1-sized random number that identifies a
 * given execution of Redis, so that if you are talking with an instance
 * having run_id == A, and you reconnect and it has run_id == B, you can be
 * sure that it is either a different instance or it was restarted. */
 /** Run ID是redis执行的一个SHA1长度的随机数, 
 如果你需要和实例(run_id=a), 当你再次连接的时候,发现run_id = b,
 则你可以确定要么连接的不是同一个实例要么重启过
 */
void getRandomHexChars(char *p, size_t len) {
    char *charset = "0123456789abcdef";
    size_t j;

    getRandomBytes((unsigned char*)p,len);
    for (j = 0; j < len; j++) p[j] = charset[p[j] & 0x0F];
}

util.c
/* Get random bytes, attempts to get an initial seed from /dev/urandom and
 * the uses a one way hash function in counter mode to generate a random
 * stream. However if /dev/urandom is not available, a weaker seed is used.
 *
 * This function is not thread safe, since the state is global. 
 * 生成随机字节
 * 尝试从/dev/urandom中获取初始化种子 & 使用一种计数的hash方法生成一个随机流
 * 如果/dev/urandom不可用，weaker seed is used.
 * 该方法不是线程安全的，尽管状态是全局的
 */
void getRandomBytes(unsigned char *p, size_t len) {
    /* Global state. */
    static int seed_initialized = 0;
    static unsigned char seed[20]; /* The SHA1 seed, from /dev/urandom. */
    static uint64_t counter = 0; /* The counter we hash with the seed. */

    if (!seed_initialized) {
        /* Initialize a seed and use SHA1 in counter mode, where we hash
         * the same seed with a progressive counter. For the goals of this
         * function we just need non-colliding strings, there are no
         * cryptographic security needs. */
        FILE *fp = fopen("/dev/urandom","r");
        if (fp == NULL || fread(seed,sizeof(seed),1,fp) != 1) {    //从文件中读取种子
            /* Revert to a weaker seed, and in this case reseed again
             * at every call.*/
            for (unsigned int j = 0; j < sizeof(seed); j++) {
                struct timeval tv;
                gettimeofday(&tv,NULL);                       //获取time of day
                pid_t pid = getpid();                         //获取当前 process ID
                seed[j] = tv.tv_sec ^ tv.tv_usec ^ pid ^ (long)fp;        //通过算法生成种子
            }
        } else {
            seed_initialized = 1;
        }
        if (fp) fclose(fp);
    }

    while(len) {
        unsigned char digest[20];
        SHA1_CTX ctx;                       /* 具体 struct 参见 sha1.h     */
        unsigned int copylen = len > 20 ? 20 : len;

        /** sha1 相关方法的实现 参见 sha1.c */
        SHA1Init(&ctx);                         
        SHA1Update(&ctx, seed, sizeof(seed));
        SHA1Update(&ctx, (unsigned char*)&counter,sizeof(counter));
        SHA1Final(digest, &ctx);
        counter++;

        memcpy(p,digest,copylen);        /*   拷贝生成好的 sha1 byte[] */
        len -= copylen;
        p += copylen;
    }
}
