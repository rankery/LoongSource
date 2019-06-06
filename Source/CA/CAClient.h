#ifndef _CACLIENT_H_
#ifdef __cplusplus
extern "C" {
#endif
	// return value: 0 valid; -1 net error; -2 ca error; -3 other
	extern int CheckValid(
		const char* pGameID,
		const char* pArea, 
		int zoneID, 
		int groupID, 
		const char* pIP, 
		const char* pCAFile,
		const char* pBlackListFile,
		const char* pCenterIP, 
		int nPort);
#ifdef __cplusplus
}
#endif 
#endif