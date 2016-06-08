/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

/* Windows needs to know explicitly which functions to export in a DLL. */
#ifdef BUILD_CJSON_AS_DLL
#	ifdef WIN32
#		define CJSON_EXPORT __declspec(dllexport)
#	elif defined(__GNUC__) && __GNUC__ >= 4
#		define CJSON_EXPORT __attribute__ ((visibility("default")))
#	else
#		define CJSON_EXPORT
#	endif
#else
#	define CJSON_EXPORT
#endif /* BUILD_CJSON_AS_DLL */

/* cJSON Types: */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6
	
#define cJSON_IsReference 256

/* The cJSON structure: */
typedef struct cJSON {
	struct cJSON *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct cJSON *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==cJSON_String */
	int valueint;				/* The item's number, if type==cJSON_Number */
	double valuedouble;			/* The item's number, if type==cJSON_Number */

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} cJSON;

typedef struct cJSON_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} cJSON_Hooks;

/* Supply malloc, realloc and free functions to cJSON */
extern CJSON_EXPORT void cJSON_InitHooks(cJSON_Hooks* hooks);

/* Uses the supplied malloc via cJSON_InitHooks. You probably never need to call this, but is here for completeness to balance cJSON_Free(). */
extern CJSON_EXPORT void* cJSON_Malloc(size_t sz);
/* Uses the supplied free via cJSON_InitHooks. Intended to be used to clean up for function like cJSON_Print() */
extern CJSON_EXPORT void cJSON_Free(void *ptr);

/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
extern CJSON_EXPORT cJSON *cJSON_Parse(const char *value);
/* Render a cJSON entity to text for transfer/storage. Free the char* with cJSON_Free() when finished. */
extern CJSON_EXPORT char  *cJSON_Print(cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* with cJSON_Free() when finished. */
extern CJSON_EXPORT char  *cJSON_PrintUnformatted(cJSON *item);
/* Delete a cJSON entity and all subentities. */
extern CJSON_EXPORT void   cJSON_Delete(cJSON *c);

/* Returns the number of items in an array (or object). */
extern CJSON_EXPORT int	  cJSON_GetArraySize(cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern CJSON_EXPORT cJSON *cJSON_GetArrayItem(cJSON *array,int item);
/* Get item "string" from object. Case insensitive. */
extern CJSON_EXPORT cJSON *cJSON_GetObjectItem(cJSON *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
extern CJSON_EXPORT const char *cJSON_GetErrorPtr(void);
	
/* These calls create a cJSON item of the appropriate type. */
extern CJSON_EXPORT cJSON *cJSON_CreateNull(void);
extern CJSON_EXPORT cJSON *cJSON_CreateTrue(void);
extern CJSON_EXPORT cJSON *cJSON_CreateFalse(void);
extern CJSON_EXPORT cJSON *cJSON_CreateBool(int b);
extern CJSON_EXPORT cJSON *cJSON_CreateNumber(double num);
extern CJSON_EXPORT cJSON *cJSON_CreateString(const char *string);
extern CJSON_EXPORT cJSON *cJSON_CreateArray(void);
extern CJSON_EXPORT cJSON *cJSON_CreateObject(void);

/* These utilities create an Array of count items. */
extern CJSON_EXPORT cJSON *cJSON_CreateIntArray(const int *numbers,int count);
extern CJSON_EXPORT cJSON *cJSON_CreateFloatArray(const float *numbers,int count);
extern CJSON_EXPORT cJSON *cJSON_CreateDoubleArray(const double *numbers,int count);
extern CJSON_EXPORT cJSON *cJSON_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern CJSON_EXPORT void cJSON_AddItemToArray(cJSON *array, cJSON *item);
extern CJSON_EXPORT void	cJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
extern CJSON_EXPORT void cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
extern CJSON_EXPORT void	cJSON_AddItemReferenceToObject(cJSON *object,const char *string,cJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
extern CJSON_EXPORT cJSON *cJSON_DetachItemFromArray(cJSON *array,int which);
extern CJSON_EXPORT void   cJSON_DeleteItemFromArray(cJSON *array,int which);
extern CJSON_EXPORT cJSON *cJSON_DetachItemFromObject(cJSON *object,const char *string);
extern CJSON_EXPORT void   cJSON_DeleteItemFromObject(cJSON *object,const char *string);
	
/* Update array items. */
extern CJSON_EXPORT void cJSON_ReplaceItemInArray(cJSON *array,int which,cJSON *newitem);
extern CJSON_EXPORT void cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);

/* Duplicate a cJSON item */
extern CJSON_EXPORT cJSON *cJSON_Duplicate(cJSON *item,int recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
extern CJSON_EXPORT cJSON *cJSON_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated);

extern CJSON_EXPORT void cJSON_Minify(char *json);

/* Macros for creating things quickly. */
#define cJSON_AddNullToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddBoolToObject(object,name,b)	cJSON_AddItemToObject(object, name, cJSON_CreateBool(b))
#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object,val)			((object)?(object)->valueint=(object)->valuedouble=(val):(val))

#ifdef __cplusplus
}
#endif

#endif
