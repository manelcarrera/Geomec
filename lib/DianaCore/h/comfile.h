 /*                                         Copyright (c) 2003 TNO DIANA B.V. */
#ifndef _COMFILE_H_
#define _COMFILE_H_

typedef struct
{
	void	*pParent;
	void	**pChildren;
	int		nChildren;
	char	*sName;
	int		iIsDir;
	int		iIsDefault;
	int		iBlockFormat;
	char	**sValue;
	int		nValues;
} DiSubDirInfo_t;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern int SaveComFile(const char *PathName, const char* InitialText, int iWriteDefaultValues);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _COMFILE_H_ */
