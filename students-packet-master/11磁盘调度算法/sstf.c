/*
提供该示例代码是为了阐释一个概念，或者进行一个测试，并不代表着
最安全的编码实践，因此不应在应用程序或网站中使用该示例代码。对
于超出本示例代码的预期用途以外的使用所造成的偶然或继发性损失，
北京英真时代科技有限公司不承担任何责任。
*/


/*
在下面的函数中实现了 SSTF (Shortest Seek Time First) 磁盘调度算法。
该算法选择这样的线程，其要求访问的磁道与当前磁头所在的磁道距离最近，
以使每次的寻道时间最短，但这种算法却不能保证平均寻道时间最短。
*/


PREQUEST
IopDiskSchedule(
	VOID
	)
{
	PLIST_ENTRY pListEntry;
	PREQUEST pRequest;
	LONG Offset;
	
	ULONG ShortestDistance = 0xFFFFFFFF;
	PREQUEST pNextRequest;
	
	//
	// 遍历请求队列选择这样的请求，其对应的线程所访问的磁道与当前磁头所在的磁道距离最近。
	//
	for (pListEntry = RequestListHead.Next;	// 请求队列中的第一个请求是链表头指向的下一个请求。
	     pListEntry != &RequestListHead;	// 遍历到请求队列头时结束循环。
	     pListEntry = pListEntry->Next) {
	
		//
		// 根据链表项获得请求的指针
		//
		pRequest = CONTAINING_RECORD(pListEntry, REQUEST, ListEntry);
		
		//
		// 计算请求对应的线程所访问的磁道与当前磁头所在磁道的偏移
		//
		Offset = pRequest->Cylinder - CurrentCylinder;
		
		//
		// 记录下最短的距离和对应的请求的指针
		// 注意，由于 ShortestDistance 被初始化为最大的无符号整型，
		// 所以在第一次循环时，条件语句的布尔表达式的值一定为真。
		//
		if (abs(Offset) < ShortestDistance) {
		
			ShortestDistance = abs(Offset);
			pNextRequest = pRequest;
		}
	}
	
	//
	// 循环结束后，pNextRequest 指向的请求对应的线程所访问
	// 的磁道与当前磁头所在的磁道距离最短。 
	//
	return pNextRequest;
}
