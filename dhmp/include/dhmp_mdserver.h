#ifndef DHMP_MDSERVER_H
#define DHMP_MDSERVER_H
struct metadata
{
	unsigned long GUID;
    char *filename;
	unsigned int node_num;

	void *out_of_date;
	void *up_to_date;
	char *dataLog;

	int r_cnt,w_cnt;
};
struct serverInfo
{
	struct dhmp_context ctx;
	struct dhmp_config config;
	struct dhmp_transport *listen_trans;

    long dram_used_size;
	long nvm_used_size;
	long dram_total_size;
	long nvm_total_size;
	
};
struct dhmp_mdserver{
    struct dhmp_context ctx;
	struct dhmp_config config;

	struct list_head dev_list;
	pthread_mutex_t mutex_server_list;
	struct list_head server_list;

	pthread_mutex_t mutex_client_list;
	struct list_head client_list;

	struct dhmp_transport *listen_trans;
	
	struct dhmp_transport *dn_trans[DHMP_SERVER_NODE_NUM];
	struct dhmp_transport *cn_trans[DHMP_SERVER_NODE_NUM];

    void* index_info;
	struct serverInfo server[DHMP_SERVER_NODE_NUM];
};

extern struct dhmp_mdserver *mdserver;

#endif
