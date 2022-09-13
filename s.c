	void **list = (void *)ptr;
	struct managed_PointerInfo 	*list_data_ptr = _mcinternal_ptrinfo(*list), datainfo, *listinfo = _mcinternal_ptrinfo(list);
	if (list_data_ptr == NULL || listinfo == NULL) return 1;
	datainfo = *list_data_ptr; /*We need the copy or else we would start accessing potentially freed memory*/
	
	_mcinternal_lock(*list);
	MC_MUTEX_LOCK(&listinfo->lock);
	if (datainfo.count >= datainfo.capacity) {
		/* 1.5 is the most efficent cap size multiplier because of the golden ratio or something like that */
		/* TODO: restudy math 11 so ~~I~dont~fail~~ I understand why the golden ratio is effective */
        size_t newcap = (size_t)((double)datainfo.capacity * 1.5), oldc = datainfo.count;
		void *newalloc = managed_allocate(newcap, datainfo.typesize, datainfo.free, NULL);
		struct managed_PointerInfo *newallocinfo = NULL; 
		if (newalloc == NULL || listinfo == NULL) return 1;

		newallocinfo = _mcinternal_ptrinfo(newalloc);

		MC_MUTEX_LOCK(&newallocinfo->lock);
		newallocinfo->count = oldc;

		MC_MEMCPY(newalloc, *list, datainfo.typesize * oldc);
		MC_MUTEX_UNLOCK(&newallocinfo->lock);
		MC_MUTEX_UNLOCK(&listinfo->lock);
		managed_release(*list);
		*list = newalloc;

		listinfo->capacity = newcap;
	}
	
	MC_MEMCPY(((unsigned char *)*list) + datainfo.count * datainfo.typesize, data, datainfo.typesize);
	
	/* Update the list fields So the semantics of mc_countof(list) work */
	listinfo->count = ++_mcinternal_ptrinfo(*list)->count;
	MC_MUTEX_UNLOCK(&listinfo->lock);
	_mcinternal_unlock(*list);
	return 0;