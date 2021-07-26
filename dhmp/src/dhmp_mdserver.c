#include "dhmp.h"
#include "dhmp_log.h"
#include "dhmp_hash.h"
#include "dhmp_config.h"
#include "dhmp_context.h"
#include "dhmp_dev.h"
#include "dhmp_transport.h"
#include "dhmp_task.h"
#include "dhmp_timerfd.h"
#include "dhmp_poll.h"
#include "dhmp_work.h"
#include "dhmp_client.h"
#include "dhmp_mdserver.h"
#include "dhmp_server.h"

/**
 *	dhmp_get_dev_from_mdserver:get the dev_ptr from dev_list of server.
 */
struct dhmp_device *dhmp_get_dev_from_mdserver()
{
	struct dhmp_device *res_dev_ptr=NULL;
	if(!list_empty(&mdserver->dev_list))
	{
		res_dev_ptr=list_first_entry(&mdserver->dev_list,
									struct dhmp_device,
									dev_entry);
	}
		
	return res_dev_ptr;
}

/**
 *	dhmp_mdserver_dispatch:dispatch the malloc request to appropriate DATAnode
 */
int dhmp_datanode_dispatch(struct dhmp_work* work){
	int node_index;
	int i;
	long freespace = 0,temp;
    if(work->work_type != DHMP_WORK_MALLOC){
		ERROR_LOG("work type error.");
		exit(-1);
	}
	for(i=0;i<DHMP_SERVER_NODE_NUM;i++){
		temp = mdserver->server[i].nvm_total_size - mdserver->server[i].nvm_used_size;
		node_index = freespace>temp?node_index:i;
	}
	return node_index;
}


void dhmp_mdserver_init(){
    int i,err=0;
	
	mdserver=(struct dhmp_mdserver *)malloc(sizeof(struct dhmp_mdserver));
	if(!mdserver)
	{
		ERROR_LOG("allocate memory error.");
		return ;
	}
	
	dhmp_hash_init();
	dhmp_config_init(&mdserver->config, false);
	dhmp_context_init(&mdserver->ctx);

	//server->watcher_trans=NULL；

	/*init client transport list*/
	
	pthread_mutex_init(&mdserver->mutex_client_list, NULL);
	INIT_LIST_HEAD(&mdserver->client_list);
	
	/*init list about rdma device*/
	INIT_LIST_HEAD(&mdserver->dev_list);
	dhmp_dev_list_init(&mdserver->dev_list);

	/*init the structure about memory count*/
	
	mdserver->listen_trans=dhmp_transport_create(&server->ctx,
											dhmp_get_dev_from_mdserver(),
											true, false);
	if(!mdserver->listen_trans)
	{
		ERROR_LOG("create rdma transport error.");
		exit(-1);
	}

	err=dhmp_transport_listen(mdserver->listen_trans,
					mdserver->config.net_infos[mdserver->config.curnet_id].port);
	if(err)
		exit(-1);
}

void dhmp_mdserver_destory(){
    INFO_LOG("mdserver destroy start.");
	pthread_join(mdserver->ctx.epoll_thread, NULL);
	INFO_LOG("mdserver destroy end.");
	free(mdserver);
}