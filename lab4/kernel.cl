__kernel void kernel_fun()
{
	int global_id, local_id, gorup_id;
	
	global_id = get_global_id(0);
	local_id = get_local_id(0);
	
	//group_id = get_group_id(0);

	printf("global_id: %d\n", global_id);
	printf("local_id: %d\n", local_id);
	//printf("group_id: %d\n", group_id);
	
}