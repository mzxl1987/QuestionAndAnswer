1.redis启动入口
1.1.在server.c中我们找到了main函数,在解读main函数的过程中遇到了很多问题.
    在注释中都已标注,并做一一解答.我想完全搞清楚了main函数对redis的启动过程也有个大概的了解
int main(int argc, char **argv) {
    struct timeval tv;
    int j;

/**
 * 定义了redis的测试部分
 */
#ifdef REDIS_TEST
    if (argc == 3 && !strcasecmp(argv[1], "test")) {
        if (!strcasecmp(argv[2], "ziplist")) {
            return ziplistTest(argc, argv);
        } else if (!strcasecmp(argv[2], "quicklist")) {
            quicklistTest(argc, argv);
        } else if (!strcasecmp(argv[2], "intset")) {
            return intsetTest(argc, argv);
        } else if (!strcasecmp(argv[2], "zipmap")) {
            return zipmapTest(argc, argv);
        } else if (!strcasecmp(argv[2], "sha1test")) {
            return sha1Test(argc, argv);
        } else if (!strcasecmp(argv[2], "util")) {
            return utilTest(argc, argv);
        } else if (!strcasecmp(argv[2], "endianconv")) {
            return endianconvTest(argc, argv);
        } else if (!strcasecmp(argv[2], "crc64")) {
            return crc64Test(argc, argv);
        } else if (!strcasecmp(argv[2], "zmalloc")) {
            return zmalloc_test(argc, argv);
        }

        return -1; /* test not found */
    }
#endif

    /* We need to initialize our libraries, and the server configuration. */
    /** 初始化我们库函数, 和服务器的配置 */
#ifdef INIT_SETPROCTITLE_REPLACEMENT
    spt_init(argc, argv);
#endif
    setlocale(LC_COLLATE,"");
    tzset(); /* Populates 'timezone' global. */
    zmalloc_set_oom_handler(redisOutOfMemoryHandler);   /* 给redisOutOfMenoryHandler 分配空间, oom = OutOfMemory */
    srand(time(NULL)^getpid());
    gettimeofday(&tv,NULL);                              /* 一天中的时间  */

    char hashseed[16];
    getRandomHexChars(hashseed,sizeof(hashseed));         /* 生成随机的hex的hash种子, 种子的作用是什么? 什么地方会用到? */
    dictSetHashFunctionSeed((uint8_t*)hashseed);
    server.sentinel_mode = checkForSentinelMode(argc,argv);   /*  检查redis的哨兵模式，不知翻译的是否恰当，作用是什么?  */
    initServerConfig();                                       /* 初始化服务器的配置, 初始化了哪些配置? 有什么作用? */
    ACLInit(); /* The ACL subsystem must be initialized ASAP because the
                  basic networking code and client creation depends on it. */
    /** ACL 子系统必须初始化为 ASAP,因为基础网络代码 & 客户端的创建都依赖于它
     *
     * ACL 是什么? ASAP 是什么? 为什么networking code & client creation 都依赖于 ACL/ASAP?
     * 
     */


    moduleInitModulesSystem();        /* module系统的初始化，module系统中包含哪些东西? */

    /* Store the executable path and arguments in a safe place in order
     * to be able to restart the server later. */
    /**  存储 可执行路径(绝对路径) & 传入的参数 到 安全区, 为了以后能够重启服务  */
    server.executable = getAbsolutePath(argv[0]);
    server.exec_argv = zmalloc(sizeof(char*)*(argc+1));
    server.exec_argv[argc] = NULL;
    for (j = 0; j < argc; j++) server.exec_argv[j] = zstrdup(argv[j]);

    /* We need to init sentinel right now as parsing the configuration file
     * in sentinel mode will have the effect of populating the sentinel
     * data structures with master nodes to monitor. */
    /**
     * 初始化哨兵通过转换配置文件，在哨兵模式下，主节点可以更有效的监视繁衍出来的哨兵。
     * 感觉翻译欠妥^_6
     */
    if (server.sentinel_mode) {
        initSentinelConfig();      /* 配置哨兵 */
        initSentinel();            /* 初始化哨兵 */
    }

    /* Check if we need to start in redis-check-rdb/aof mode. We just execute
     * the program main. However the program is part of the Redis executable
     * so that we can easily execute an RDB check on loading errors. */
    /**  什么事 rdb mode & aof mode ?  */
    if (strstr(argv[0],"redis-check-rdb") != NULL)
        redis_check_rdb_main(argc,argv,NULL);               /** check 的时候做了哪些事 */
    else if (strstr(argv[0],"redis-check-aof") != NULL)
        redis_check_aof_main(argc,argv);                    /** check 的时候做了哪些事 */

    /** 解析输入的指令   */
    if (argc >= 2) {
        j = 1; /* First option to parse in argv[] */
        sds options = sdsempty();
        char *configfile = NULL;

        /* Handle special options --help and --version */
        if (strcmp(argv[1], "-v") == 0 ||
            strcmp(argv[1], "--version") == 0) version();
        if (strcmp(argv[1], "--help") == 0 ||
            strcmp(argv[1], "-h") == 0) usage();
        if (strcmp(argv[1], "--test-memory") == 0) {
            if (argc == 3) {
                memtest(atoi(argv[2]),50);
                exit(0);
            } else {
                fprintf(stderr,"Please specify the amount of memory to test in megabytes.\n");
                fprintf(stderr,"Example: ./redis-server --test-memory 4096\n\n");
                exit(1);
            }
        }

        /* First argument is the config file name? */
        if (argv[j][0] != '-' || argv[j][1] != '-') {
            configfile = argv[j];
            server.configfile = getAbsolutePath(configfile);
            /* Replace the config file in server.exec_argv with
             * its absolute path. */
            zfree(server.exec_argv[j]);
            server.exec_argv[j] = zstrdup(server.configfile);
            j++;
        }

        /* All the other options are parsed and conceptually appended to the
         * configuration file. For instance --port 6380 will generate the
         * string "port 6380\n" to be parsed after the actual file name
         * is parsed, if any. */
        while(j != argc) {
            if (argv[j][0] == '-' && argv[j][1] == '-') {
                /* Option name */
                if (!strcmp(argv[j], "--check-rdb")) {
                    /* Argument has no options, need to skip for parsing. */
                    j++;
                    continue;
                }
                if (sdslen(options)) options = sdscat(options,"\n");
                options = sdscat(options,argv[j]+2);
                options = sdscat(options," ");
            } else {
                /* Option argument */
                options = sdscatrepr(options,argv[j],strlen(argv[j]));
                options = sdscat(options," ");
            }
            j++;
        }
        if (server.sentinel_mode && configfile && *configfile == '-') {
            serverLog(LL_WARNING,
                "Sentinel config from STDIN not allowed.");
            serverLog(LL_WARNING,
                "Sentinel needs config file on disk to save state.  Exiting...");
            exit(1);
        }
        resetServerSaveParams();
        loadServerConfig(configfile,options);
        sdsfree(options);
    }

    serverLog(LL_WARNING, "oO0OoO0OoO0Oo Redis is starting oO0OoO0OoO0Oo");
    serverLog(LL_WARNING,
        "Redis version=%s, bits=%d, commit=%s, modified=%d, pid=%d, just started",
            REDIS_VERSION,
            (sizeof(long) == 8) ? 64 : 32,
            redisGitSHA1(),
            strtol(redisGitDirty(),NULL,10) > 0,
            (int)getpid());

    if (argc == 1) {
        serverLog(LL_WARNING, "Warning: no config file specified, using the default config. In order to specify a config file use %s /path/to/%s.conf", argv[0], server.sentinel_mode ? "sentinel" : "redis");
    } else {
        serverLog(LL_WARNING, "Configuration loaded");
    }

    /** 监督模式  */
    server.supervised = redisIsSupervised(server.supervised_mode);
    int background = server.daemonize && !server.supervised;
    if (background) daemonize();

    initServer();           /** 初始化服务器, 具体初始化了什么?  */
    if (background || server.pidfile) createPidFile();       /** 创建Pid文件  */
    redisSetProcTitle(argv[0]);          /** 设置进程 title , proc = process */
    redisAsciiArt();                     /** 用ascii码打印logo */
    checkTcpBacklogSettings();           /** 检查tcp后台日志的设置, 设置了哪些东西? **/

    if (!server.sentinel_mode) {       /** 哨兵模式下的一些设置  */
        /* Things not needed when running in Sentinel mode. */
        serverLog(LL_WARNING,"Server initialized");
    #ifdef __linux__
        linuxMemoryWarnings();
    #endif
        moduleLoadFromQueue();           /** 从队列中加载module, 加载了哪些module? module的作用是什么? */
        ACLLoadUsersAtStartup();         /** ACL 加载用户 在启动的时候, 如何加载用户的? */
        loadDataFromDisk();              /** 从磁盘加载数据 , 如何加载数据的? 加载了哪些数据? */
        if (server.cluster_enabled) {
            if (verifyClusterConfigWithData() == C_ERR) {   /**通过数据验证集群的配置， 如何验证的? */
                serverLog(LL_WARNING,
                    "You can't have keys in a DB different than DB 0 when in "
                    "Cluster mode. Exiting.");
                exit(1);
            }
        }
        if (server.ipfd_count > 0)
            serverLog(LL_NOTICE,"Ready to accept connections");
        if (server.sofd > 0)
            serverLog(LL_NOTICE,"The server is now ready to accept connections at %s", server.unixsocket);
    } else {
        sentinelIsRunning();       /**   处于哨兵模式下时候， 该函数做了哪些事情? */
    }

    /* Warning the user about suspicious maxmemory setting. */
    if (server.maxmemory > 0 && server.maxmemory < 1024*1024) {
        serverLog(LL_WARNING,"WARNING: You specified a maxmemory value that is less than 1MB (current value is %llu bytes). Are you sure this is what you really want?", server.maxmemory);
    }

    aeSetBeforeSleepProc(server.el,beforeSleep);   /** 设置进程休眠的 before event */   
    aeSetAfterSleepProc(server.el,afterSleep);     /** 设置进程休眠的 after event */
    aeMain(server.el);                             /** 设置event loop 的主要方法 */
    aeDeleteEventLoop(server.el);                  /** 删除 event loop */
    return 0;
}
