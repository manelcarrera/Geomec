#ifndef _GLIB_STUB_H_
#define _GLIB_STUB_H_

#include <float.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNUSED
# define UNUSED(x) (void)(x)
#endif  // UNUSED

#define G_GNUC_FUNCTION         ""
#define G_GNUC_PRETTY_FUNCTION  ""

#ifndef NULL
#  ifdef __cplusplus
#    define NULL        (0L)
#  else /* !__cplusplus */
#    define NULL        ((void*) 0)
#  endif /* !__cplusplus */
#endif

#define g_return_if_fail(expr) \
  if(!(expr)) return

#define g_return_val_if_fail(expr, val) \
  if(!(expr)) return val

#define G_MAXDOUBLE DBL_MAX
#define G_MAXFLOAT FLT_MAX
#define G_MAXINT INT_MAX

#define TRUE (1)
#define FALSE (0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifdef _DEBUG
#define g_assert(expr) assert(expr)
#else
#define g_assert(expr) (expr)
#endif

#define g_assert_not_reached() g_assert(0)

#define g_mem_chunk_create(type, pre_alloc, alloc_type)	( \
  g_mem_chunk_new (#type " mem chunks (" #pre_alloc ")", \
		   sizeof (type), \
		   sizeof (type) * (pre_alloc), \
		   (alloc_type)) \
)

#define g_chunk_new(type, chunk)	( \
  (type *) g_mem_chunk_alloc (chunk) \
)

#define g_malloc(size)	     ((gpointer) malloc (size))
#define g_malloc0(size)	     ((gpointer) calloc (size, sizeof(char)))
#define g_free(mem)	     free (mem)

#define G_ALLOC_ONLY	  1
#define G_ALLOC_AND_FREE  2

/* Provide simple macro statement wrappers (adapted from Perl):
 *  G_STMT_START { statements; } G_STMT_END;
 *  can be used as a single statement, as in
 *  if (x) G_STMT_START { ... } G_STMT_END; else ...
 *
 *  For gcc we will wrap the statements within `({' and `})' braces.
 *  For SunOS they will be wrapped within `if (1)' and `else (void) 0',
 *  and otherwise within `do' and `while (0)'.
 */
#if !(defined (G_STMT_START) && defined (G_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define G_STMT_START	(void)(
#    define G_STMT_END		)
#  else
#    if (defined (sun) || defined (__sun__))
#      define G_STMT_START	if (1)
#      define G_STMT_END	else (void)0
#    else
#      define G_STMT_START	do
#      define G_STMT_END	while (0)
#    endif
#  endif
#endif

#define GUINT_TO_POINTER(u) ((gpointer)(u))
#define GPOINTER_TO_UINT(u) ((guint)(u))
#define GINT_TO_POINTER(i)  ((gpointer)(i))

typedef unsigned long long guint;
typedef double gdouble;
typedef void* gpointer;
typedef const void* gconstpointer;
typedef float gfloat;
typedef long long gint;
typedef char gchar;
typedef unsigned int guint32;
typedef long long gboolean;
typedef unsigned long gulong;

typedef guint           (*GHashFunc)            (gconstpointer  key);
typedef gboolean        (*GEqualFunc)           (gconstpointer  a,
                                                 gconstpointer  b);
typedef void            (*GDestroyNotify)       (gpointer       data);

typedef gint            (*GCompareFunc)         (gconstpointer  a,
                                                 gconstpointer  b);

typedef void            (*GFunc)                (gpointer       data,
                                                 gpointer       user_data);
typedef void            (*GHFunc)               (gpointer       key,
                                                 gpointer       value,
                                                 gpointer       user_data);
typedef gboolean	      (*GHRFunc)		          (gpointer	      key,
						                                     gpointer	      value,
						                                     gpointer	      user_data);
typedef void            (*GFreeFunc)            (gpointer       data);

void g_warning(const char* txt, ...);

typedef struct _GString		GString;

struct _GString
{
  gchar *str;
  gint len;
};

GString*     g_string_new	        (const gchar	 *init);
gchar*	     g_string_free	        (GString	 *string,
					 gboolean	  free_segment);
GString*     g_string_append_c          (GString	 *string,
					 gchar		  c);

typedef struct _GSList		GSList;

struct _GSList
{
  gpointer data;
  GSList *next;
};

GSList*  g_slist_append         (GSList           *list,
				                         gpointer          data);
GSList*  g_slist_alloc          (void);
void     g_slist_free           (GSList           *list);
GSList*  g_slist_remove         (GSList           *list,
				                         gconstpointer     data);
guint    g_slist_length         (GSList           *list);
GSList*  g_slist_find           (GSList           *list,
				 gconstpointer     data);
GSList*  g_slist_prepend        (GSList           *list,
				                         gpointer          data);
void     g_slist_foreach        (GSList           *list,
				                         GFunc             func,
				                         gpointer          user_data);
GSList*  g_slist_nth            (GSList           *list,
				                         guint             n);
GSList*  g_slist_concat         (GSList           *list1,
				                         GSList           *list2);
void     g_slist_free_1         (GSList           *list);
GSList*  g_slist_copy           (GSList           *list);
GSList*  g_slist_reverse        (GSList           *list);
GSList*  g_slist_remove_link    (GSList           *list,
				                         GSList           *link);
GSList*  g_slist_last           (GSList *list);

typedef struct _GList		GList;

struct _GList
{
  gpointer data;
  GList *next;
  GList *prev;
};

void     g_list_free           (GList            *list);
void     g_list_free_1         (GList            *list);
GList*   g_list_remove_link    (GList            *list,
				                        GList            *link);
GList*   g_list_prepend        (GList            *list,
				                        gpointer          data);
guint    g_list_length         (GList            *list);

typedef struct _GNode		GNode;

struct _GNode
{
  gpointer data;
  GNode	  *next;
  GNode	  *prev;
  GNode	  *parent;
  GNode	  *children;
};

#define	 G_NODE_IS_ROOT(node)	(((GNode*) (node))->parent == NULL && \
				 ((GNode*) (node))->prev == NULL && \
				 ((GNode*) (node))->next == NULL)

GNode*	 g_node_new		(gpointer	   data);
GNode*	 g_node_prepend		(GNode		  *parent,
				                   GNode		  *node);
guint	 g_node_depth		(GNode		  *node);
void	 g_node_destroy		(GNode		  *root);
GNode* g_node_insert_before (GNode *parent,
		                         GNode *sibling,
		                         GNode *node);
void g_node_unlink (GNode *node);

typedef struct _GPtrArray	GPtrArray;

struct _GPtrArray
{
  gpointer *pdata;
  guint	    len;
};

#define    g_ptr_array_index(array,index) (array->pdata)[index]

GPtrArray* g_ptr_array_new                (void);
gpointer*  g_ptr_array_free               (GPtrArray        *array,
					                                 gboolean          free_seg);
void       g_ptr_array_add                (GPtrArray        *array,
					                                 gpointer          data);
gpointer   g_ptr_array_remove_index       (GPtrArray        *array,
					                                 guint             index);
void       g_ptr_array_set_size           (GPtrArray        *array,
					                                 gint              length);

typedef struct _GArray		GArray;

struct _GArray
{
  gchar *data;
  guint len;
};

#define g_array_append_val(a,v)	  g_array_append_vals (a, &v, 1)
#define g_array_index(a,t,i)      (((t*) (a)->data) [(i)])

GArray* g_array_new               (gboolean          zero_terminated,
				                           gboolean          clear,
				                           guint             element_size);
GArray* g_array_sized_new         (gboolean          zero_terminated,
			                             gboolean          clear,
			                             guint             elt_size,
			                             guint             reserved_size);
gchar*  g_array_free              (GArray           *array,
				                           gboolean          free_segment);
GArray* g_array_append_vals       (GArray           *array,
				                           gconstpointer     data,
				                           guint             len);

typedef struct _GHashTable	GHashTable;

GHashTable* g_hash_table_new		(GHashFunc	 hash_func,
					                       GEqualFunc	 key_equal_func);
GHashTable* g_hash_table_new_full (GHashFunc       hash_func,
		                               GEqualFunc      key_equal_func,
		                               GDestroyNotify  key_destroy_func,
		                               GDestroyNotify  value_destroy_func);
void	    g_hash_table_destroy	(GHashTable	*hash_table);
void    g_hash_table_remove_all (GHashTable *hash_table);
void   g_hash_table_unref (GHashTable *hash_table);
void	    g_hash_table_insert		(GHashTable	*hash_table,
					                       gpointer	 key,
					                       gpointer	 value);
gboolean    g_hash_table_remove		(GHashTable	*hash_table,
					                         gconstpointer	 key);
guint	    g_hash_table_size		(GHashTable	*hash_table);
void	    g_hash_table_foreach	(GHashTable	*hash_table,
					                       GHFunc		 func,
					                       gpointer	 user_data);
gpointer    g_hash_table_lookup		(GHashTable	*hash_table,
					                         gconstpointer	 key);
guint	    g_hash_table_foreach_remove	(GHashTable	*hash_table,
					                             GHRFunc	 func,
					                             gpointer	 user_data);

guint    g_str_hash  (gconstpointer   v);
gboolean g_str_equal (gconstpointer   v,
                      gconstpointer   v2);

/* Tree traverse orders */
typedef enum
{
  G_IN_ORDER,
  G_PRE_ORDER,
  G_POST_ORDER,
  G_LEVEL_ORDER
} GTraverseType;

typedef struct _GMemChunk       GMemChunk;

GMemChunk* g_mem_chunk_new     (gchar	  *name,
				                        gint	   atom_size,
				                        gulong	   area_size,
				                        gint	   type);
gpointer   g_mem_chunk_alloc   (GMemChunk *mem_chunk);
void	   g_mem_chunk_free    (GMemChunk *mem_chunk,
				                      gpointer   mem);
void	   g_mem_chunk_destroy (GMemChunk *mem_chunk);

#ifdef __cplusplus
}
#endif

#endif // _GLIB_STUB_H_