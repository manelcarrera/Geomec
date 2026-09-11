#include "glib-stub.h"

#ifdef _WIN32
#include "suppress_warnings.h"
#endif

#include <string.h>

// General

void g_warning(const char* txt, ...)
{
  UNUSED(txt);
}


// GString

GString* g_string_new(const gchar	*init)
{
  GString* string = malloc(sizeof(GString));
  string->len = (init != NULL ? (int)strlen(init) : 0);
  if(string->len)
  {
    string->str = malloc(string->len * sizeof(gchar));
    strcpy(string->str, init);
  }
  else
  {
    string->str = NULL;
  }

  return string;
}

gchar* g_string_free(GString	 *string,
					           gboolean	  free_segment)
{
  gchar* ret = NULL;

  if(string == NULL)
    return NULL;

  if(free_segment)
    free(string->str);
  else
    ret = string->str;

  free(string);

  return ret;
}

GString* g_string_append_c(GString	 *string,
					                 gchar		  c)
{
  gchar* str;

  if(string == NULL)
    return NULL;

  str = malloc(string->len + 1);
  strcpy(str, string->str);
  str[string->len] = c;

  free(string->str);
  string->str = str;

  ++string->len;

  return string;
}

// GSList

GSList* g_slist_append(GSList           *list,
				               gpointer          data)
{
  GSList* l = g_slist_last(list);
  GSList* n = g_slist_alloc();

  if(l)
  {
    l->next = n;
  }

  n->data = data;

  return n;
}

GSList* g_slist_alloc(void)
{
  GSList* l = malloc(sizeof(GSList));
  l->data = NULL;
  l->next = NULL;

  return l;
}

void g_slist_free(GSList *list)
{
  if(list)
  {
    // The scenario given in Mantis 3255 resulted in a stack overflow,
    // hence the repetitive solution instead of the recursive one.

    GSList* nextNode = NULL;

    while (list != NULL) {
      nextNode = list->next;
      g_slist_free_1(list);
      list = nextNode;
    }
  }
}

void g_slist_free_1(GSList           *list)
{
  free(list);
}

GSList* g_slist_remove(GSList           *list,
				               gconstpointer     data)
{
  GSList *tmp, *prev = NULL;

  tmp = list;
  while (tmp)
  {
    if (tmp->data == data)
	  {
	    if (prev)
	      prev->next = tmp->next;
	    else
	      list = tmp->next;

	    g_slist_free_1(tmp);
	    break;
	  }
    prev = tmp;
    tmp = prev->next;
  }

  return list;
}

guint g_slist_length(GSList           *list)
{
  guint length;

  length = 0;
  while (list)
    {
      length++;
      list = list->next;
    }

  return length;
}

GSList* g_slist_find(GSList           *list,
				             gconstpointer     data)
{
  while (list)
  {
    if (list->data == data)
      break;
    list = list->next;
  }

  return list;
}

GSList* g_slist_prepend(GSList           *list,
				                gpointer          data)
{
  GSList *new_list;

  new_list = g_slist_alloc ();
  new_list->data = data;
  new_list->next = list;

  return new_list;
}

void g_slist_foreach(GSList           *list,
				             GFunc             func,
				             gpointer          user_data)
{
  while (list)
  {
    GSList *next = list->next;
    (*func) (list->data, user_data);
    list = next;
  }
}

GSList* g_slist_nth(GSList           *list,
				            guint             n)
{
  while (n-- > 0 && list)
    list = list->next;

  return list;
}

GSList* g_slist_concat(GSList           *list1,
				               GSList           *list2)
{
  if (list2)
  {
    if (list1)
      g_slist_last (list1)->next = list2;
    else
      list1 = list2;
  }

  return list1;
}

GSList* g_slist_copy(GSList           *list)
{
  GSList *new_list = NULL;

  if (list)
  {
    GSList *last;

    new_list = g_slist_alloc ();
    new_list->data = list->data;
    last = new_list;
    list = list->next;
    while (list)
    {
      last->next = g_slist_alloc ();
      last = last->next;
      last->data = list->data;
      list = list->next;
    }
    last->next = NULL;
  }

  return new_list;
}

GSList* g_slist_reverse(GSList           *list)
{
  GSList *prev = NULL;
  
  while (list)
  {
    GSList *next = list->next;

    list->next = prev;
    
    prev = list;
    list = next;
  }
  
  return prev;
}

GSList* g_slist_remove_link(GSList           *list,
				                    GSList           *link)
{
  GSList *tmp;
  GSList *prev;

  prev = NULL;
  tmp = list;

  while (tmp)
  {
    if (tmp == link)
    {
      if (prev)
        prev->next = tmp->next;
      if (list == tmp)
        list = list->next;

      tmp->next = NULL;
      break;
    }

    prev = tmp;
    tmp = tmp->next;
  }

  return list;
}

GSList* g_slist_last(GSList *list)
{
  if (list)
    {
      while (list->next)
	      list = list->next;
    }

  return list;
}

// GList

static GList* _g_list_alloc()
{
  GList* l = malloc(sizeof(GList));
  l->data = NULL;
  l->prev = NULL;
  l->next = NULL;

  return l;
}

void g_list_free(GList *list)
{
  if(list)
  {
    if(list->next)
      g_list_free(list->next);

    g_list_free_1(list);
  }
}

void g_list_free_1(GList *list)
{
  free(list);
}

GList* g_list_remove_link(GList            *list,
				                  GList            *link)
{
  if (link)
  {
    if (link->prev)
      link->prev->next = link->next;
    if (link->next)
      link->next->prev = link->prev;
    
    if (link == list)
      list = list->next;
    
    link->next = NULL;
    link->prev = NULL;
  }
  
  return list;
}

GList* g_list_prepend(GList            *list,
				              gpointer          data)
{
  GList *new_list;
  
  new_list = _g_list_alloc ();
  new_list->data = data;
  new_list->next = list;
  
  if (list)
  {
    new_list->prev = list->prev;
    if (list->prev)
      list->prev->next = new_list;
    list->prev = new_list;
  }
  else
    new_list->prev = NULL;
  
  return new_list;
}

guint g_list_length(GList *list)
{
  guint length;
  
  length = 0;
  while (list)
  {
    length++;
    list = list->next;
  }
  
  return length;
}

// GNode

GNode* g_node_new(gpointer	data)
{
  GNode* n = malloc(sizeof(GNode));
  n->data = data;
  n->children = NULL;
  n->next = NULL;
  n->prev = NULL;
  n->parent = NULL;

  return n;
}

static void
g_nodes_free (GNode *node)
{
  while (node)
    {
      GNode *next = node->next;
      if (node->children)
        g_nodes_free (node->children);
      free(node);
      node = next;
    }
}

GNode* g_node_prepend(GNode		  *parent,
				              GNode		  *node)
{
  g_return_val_if_fail (parent != NULL, node);
  
  return g_node_insert_before (parent, parent->children, node);
}

guint	g_node_depth(GNode *node)
{
  register guint depth = 0;
  
  while (node)
  {
    depth++;
    node = node->parent;
  }
  
  return depth;
}

void g_node_destroy(GNode *root)
{
  g_return_if_fail (root != NULL);
  
  if (!G_NODE_IS_ROOT (root))
    g_node_unlink (root);
  
  g_nodes_free (root);
}

GNode* g_node_insert_before (GNode *parent,
		                         GNode *sibling,
		                         GNode *node)
{
  g_return_val_if_fail (parent != NULL, node);
  g_return_val_if_fail (node != NULL, node);
  g_return_val_if_fail (G_NODE_IS_ROOT (node), node);
  if (sibling)
    g_return_val_if_fail (sibling->parent == parent, node);
  
  node->parent = parent;
  
  if (sibling)
  {
    if (sibling->prev)
	  {
	    node->prev = sibling->prev;
	    node->prev->next = node;
	    node->next = sibling;
	    sibling->prev = node;
	  }
    else
	  {
	    node->parent->children = node;
	    node->next = sibling;
	    sibling->prev = node;
	  }
  }
  else
  {
    if (parent->children)
    {
      sibling = parent->children;
      while (sibling->next)
        sibling = sibling->next;
      node->prev = sibling;
      sibling->next = node;
    }
    else
      node->parent->children = node;
  }

  return node;
}

void g_node_unlink (GNode *node)
{
  g_return_if_fail (node != NULL);
  
  if (node->prev)
    node->prev->next = node->next;
  else if (node->parent)
    node->parent->children = node->next;
  node->parent = NULL;
  if (node->next)
    {
      node->next->prev = node->prev;
      node->next = NULL;
    }
  node->prev = NULL;
}

// GPtrArray

GPtrArray* g_ptr_array_new(void)
{
  GPtrArray* ar = malloc(sizeof(GPtrArray));
  ar->len = 0;
  ar->pdata = 0;

  return ar;
}

gpointer* g_ptr_array_free(GPtrArray        *array,
					                 gboolean          free_seg)
{
  gpointer pdata;

  g_return_val_if_fail (array, NULL);

  pdata = array->pdata;

  if(free_seg)
  {
    free(pdata);
    pdata = NULL;
  }

  free(array);

  return pdata;
}

void g_ptr_array_add(GPtrArray        *array,
					           gpointer          data)
{
  g_ptr_array_set_size(array, array->len + 1);
  array->pdata[array->len - 1] = data;
}

gpointer g_ptr_array_remove_index(GPtrArray        *array,
					                        guint             index)
{
  gpointer ret;
  guint i;

  g_return_val_if_fail (array, NULL);
  g_return_val_if_fail (index < array->len, NULL);

  ret = array->pdata[index];

  for(i = index + 1; i < array->len; ++i)
    array->pdata[i - 1] = array->pdata[i];

  array->pdata[--array->len] = NULL;

  return ret;
}

void g_ptr_array_set_size(GPtrArray        *array,
					                gint              length)
{
  g_return_if_fail(array);

  if(length > (gint)array->len)
  {
    gint i;
    array->pdata = realloc(array->pdata, length * sizeof(gpointer));
    for(i = array->len; i < length; ++i)
      array->pdata[i] = NULL;
  }
  else if(length < (gint)array->len)
  {
    gint i;
    for(i = length; i < (gint)array->len; ++i)
      array->pdata[i] = NULL;
  }

  array->len = length;
}

// GArray

#define MIN_ARRAY_SIZE  16

typedef struct _GRealArray  GRealArray;

struct _GRealArray
{
  gchar  *data;
  guint   len;
  guint   alloc;
  guint   elt_size;
  guint   zero_terminated : 1;
  guint   clear : 1;
};

#define g_array_elt_len(array,i) ((array)->elt_size * (i))
#define g_array_elt_pos(array,i) ((array)->data + g_array_elt_len((array),(i)))
#define g_array_elt_zero(array, pos, len) 				\
  (memset (g_array_elt_pos ((array), pos), 0,  g_array_elt_len ((array), len)))
#define g_array_zero_terminate(array) G_STMT_START{			\
  if ((array)->zero_terminated)						\
    g_array_elt_zero ((array), (array)->len, 1);			\
}G_STMT_END

static gint
g_nearest_pow (gint num)
{
  gint n = 1;

  while (n < num)
    n <<= 1;

  return n;
}

static void
g_array_maybe_expand (GRealArray *array,
		                  gint        len)
{
  guint want_alloc = g_array_elt_len (array, array->len + len + 
				      array->zero_terminated);

  if (want_alloc > array->alloc)
    {
      want_alloc = g_nearest_pow (want_alloc);
      want_alloc = MAX (want_alloc, MIN_ARRAY_SIZE);

      array->data = realloc (array->data, want_alloc);

      memset (array->data + array->alloc, 0, want_alloc - array->alloc);

      array->alloc = want_alloc;
    }
}

GArray* g_array_new(gboolean          zero_terminated,
				            gboolean          clear,
				            guint             element_size)
{
  return (GArray*) g_array_sized_new (zero_terminated, clear, element_size, 0);
}

GArray* g_array_sized_new (gboolean zero_terminated,
			   gboolean clear,
			   guint    elt_size,
			   guint    reserved_size)
{
  GRealArray *array = malloc(sizeof(GRealArray));

  array->data            = NULL;
  array->len             = 0;
  array->alloc           = 0;
  array->zero_terminated = (zero_terminated ? 1 : 0);
  array->clear           = (clear ? 1 : 0);
  array->elt_size        = elt_size;

  if (array->zero_terminated || reserved_size != 0)
  {
    g_array_maybe_expand (array, reserved_size);
    g_array_zero_terminate(array);
  }

  return (GArray*) array;
}

gchar* g_array_free(GArray           *array,
				            gboolean          free_segment)
{
  gchar* ret;

  g_return_val_if_fail(array, NULL);

  ret = array->data;

  if(free_segment)
  {
    free(ret);
    ret = NULL;
  }

  free(array);

  return ret;
}

GArray* g_array_append_vals(GArray           *farray,
				                    gconstpointer     data,
				                    guint             len)
{
  GRealArray *array = (GRealArray*) farray;

  g_array_maybe_expand (array, len);

  memcpy (g_array_elt_pos (array, array->len), data, 
	  g_array_elt_len (array, len));

  array->len += len;

  g_array_zero_terminate (array);

  return farray;
}

// GHashTable

typedef struct _GHashNode      GHashNode;

struct _GHashNode
{
  gpointer   key;
  gpointer   value;
  GHashNode *next;
};

struct _GHashTable
{
  gint             size;
  gint             nnodes;
  GHashNode      **nodes;
  GHashFunc        hash_func;
  GEqualFunc       key_equal_func;
  volatile gint    ref_count;
  GDestroyNotify   key_destroy_func;
  GDestroyNotify   value_destroy_func;
};

#define HASH_TABLE_MIN_SIZE 11
#define HASH_TABLE_MAX_SIZE 13845163
#define G_HASH_TABLE_RESIZE(hash_table)				\
   G_STMT_START {						\
     if ((hash_table->size >= 3 * hash_table->nnodes &&	        \
	  hash_table->size > HASH_TABLE_MIN_SIZE) ||		\
	 (3 * hash_table->size <= hash_table->nnodes &&	        \
	  hash_table->size < HASH_TABLE_MAX_SIZE))		\
	   g_hash_table_resize (hash_table);			\
   } G_STMT_END

static const guint g_primes[] =
{
  11,
  19,
  37,
  73,
  109,
  163,
  251,
  367,
  557,
  823,
  1237,
  1861,
  2777,
  4177,
  6247,
  9371,
  14057,
  21089,
  31627,
  47431,
  71143,
  106721,
  160073,
  240101,
  360163,
  540217,
  810343,
  1215497,
  1823231,
  2734867,
  4102283,
  6153409,
  9230113,
  13845163,
};

static const guint g_nprimes = sizeof (g_primes) / sizeof (g_primes[0]);

guint g_spaced_primes_closest (guint num)
{
  guint i;

  for (i = 0; i < g_nprimes; i++)
    if (g_primes[i] > num)
      return g_primes[i];

  return g_primes[g_nprimes - 1];
}

static guint g_direct_hash(gconstpointer v)
{
  return GPOINTER_TO_UINT (v);
}

static void g_hash_nodes_destroy (GHashNode *hash_node,
		                              GFreeFunc  key_destroy_func,
		                              GFreeFunc  value_destroy_func)
{
  while (hash_node)
  {
    GHashNode *next = hash_node->next;
    if (key_destroy_func)
      key_destroy_func (hash_node->key);
    if (value_destroy_func)
      value_destroy_func (hash_node->value);
    free(hash_node);
    hash_node = next;
  }
}

static void
g_hash_table_resize (GHashTable *hash_table)
{
  GHashNode **new_nodes;
  GHashNode *node;
  GHashNode *next;
  guint hash_val;
  gint new_size;
  gint i;

  new_size = g_spaced_primes_closest (hash_table->nnodes);
  new_size = CLAMP (new_size, HASH_TABLE_MIN_SIZE, HASH_TABLE_MAX_SIZE);
 
  new_nodes = malloc(sizeof(GHashNode*) * new_size);
  for(i = 0; i < new_size; ++i)
    new_nodes[i] = NULL;
  
  for (i = 0; i < hash_table->size; i++)
    for (node = hash_table->nodes[i]; node; node = next)
    {
      next = node->next;

      hash_val = (* hash_table->hash_func) (node->key) % new_size;

      node->next = new_nodes[hash_val];
      new_nodes[hash_val] = node;
    }
  
  free(hash_table->nodes);
  hash_table->nodes = new_nodes;
  hash_table->size = new_size;
}

static GHashNode**
g_hash_table_lookup_node (GHashTable	*hash_table,
			  gconstpointer	 key)
{
  GHashNode **node;
  
  node = &hash_table->nodes
    [(* hash_table->hash_func) (key) % hash_table->size];
  
  /* Hash table lookup needs to be fast.
   *  We therefore remove the extra conditional of testing
   *  whether to call the key_equal_func or not from
   *  the inner loop.
   */
  if (hash_table->key_equal_func)
    while (*node && !(*hash_table->key_equal_func) ((*node)->key, key))
      node = &(*node)->next;
  else
    while (*node && (*node)->key != key)
      node = &(*node)->next;
  
  return node;
}

static GHashNode*
g_hash_node_new (gpointer key,
		 gpointer value)
{
  GHashNode *hash_node = malloc(sizeof(GHashNode));
  
  hash_node->key = key;
  hash_node->value = value;
  hash_node->next = NULL;
  
  return hash_node;
}

static void
g_hash_node_destroy (GHashNode      *hash_node,
		     GDestroyNotify  key_destroy_func,
		     GDestroyNotify  value_destroy_func)
{
  if (key_destroy_func)
    key_destroy_func (hash_node->key);
  if (value_destroy_func)
    value_destroy_func (hash_node->value);
  free(hash_node);
}

static guint
g_hash_table_foreach_remove_or_steal (GHashTable *hash_table,
                                      GHRFunc	  func,
                                      gpointer	  user_data,
                                      gboolean    notify)
{
  GHashNode *node, *prev;
  gint i;
  guint deleted = 0;
  
  for (i = 0; i < hash_table->size; i++)
    {
    restart:
      
      prev = NULL;
      
      for (node = hash_table->nodes[i]; node; prev = node, node = node->next)
	{
	  if ((* func) (node->key, node->value, user_data))
	    {
	      deleted += 1;
	      
	      hash_table->nnodes -= 1;
	      
	      if (prev)
		{
		  prev->next = node->next;
		  g_hash_node_destroy (node,
				       notify ? hash_table->key_destroy_func : NULL,
				       notify ? hash_table->value_destroy_func : NULL);
		  node = prev;
		}
	      else
		{
		  hash_table->nodes[i] = node->next;
		  g_hash_node_destroy (node,
				       notify ? hash_table->key_destroy_func : NULL,
				       notify ? hash_table->value_destroy_func : NULL);
		  goto restart;
		}
	    }
	}
    }
  
  G_HASH_TABLE_RESIZE (hash_table);
  
  return deleted;
}

GHashTable* g_hash_table_new(GHashFunc	 hash_func,
					                   GEqualFunc	 key_equal_func)
{
  return g_hash_table_new_full (hash_func, key_equal_func, NULL, NULL);
}

GHashTable*
g_hash_table_new_full (GHashFunc       hash_func,
		                   GEqualFunc      key_equal_func,
		                   GDestroyNotify  key_destroy_func,
		                   GDestroyNotify  value_destroy_func)
{
  GHashTable *hash_table;
  int i;
  
  hash_table = malloc(sizeof(GHashTable));
  hash_table->size               = HASH_TABLE_MIN_SIZE;
  hash_table->nnodes             = 0;
  hash_table->hash_func          = hash_func ? hash_func : g_direct_hash;
  hash_table->key_equal_func     = key_equal_func;
  hash_table->ref_count          = 1;
  hash_table->key_destroy_func   = key_destroy_func;
  hash_table->value_destroy_func = value_destroy_func;
  hash_table->nodes              = malloc(sizeof(GHashNode*) * hash_table->size);

  for(i = 0; i < hash_table->size; ++i)
    hash_table->nodes[i] = 0;
  
  return hash_table;
}

void g_hash_table_destroy(GHashTable	*hash_table)
{
  g_return_if_fail (hash_table != NULL);
  g_return_if_fail (hash_table->ref_count > 0);
  
  g_hash_table_remove_all (hash_table);
  g_hash_table_unref (hash_table);
}

void g_hash_table_remove_all (GHashTable *hash_table)
{
  gint i;

  g_return_if_fail (hash_table != NULL);

  for (i = 0; i < hash_table->size; i++)
  {
    g_hash_nodes_destroy (hash_table->nodes[i],
                          hash_table->key_destroy_func,
                          hash_table->value_destroy_func);
    hash_table->nodes[i] = NULL;
  }
  hash_table->nnodes = 0;
  
  G_HASH_TABLE_RESIZE (hash_table);
}

void g_hash_table_unref (GHashTable *hash_table)
{
  g_return_if_fail (hash_table != NULL);
  g_return_if_fail (hash_table->ref_count > 0);

  if ((hash_table->ref_count - 1) - 1 == 0)
  {
    gint i;

    for (i = 0; i < hash_table->size; i++)
      g_hash_nodes_destroy (hash_table->nodes[i], 
                            hash_table->key_destroy_func,
                            hash_table->value_destroy_func);
    g_free (hash_table->nodes);
    free(hash_table);
  }
}

void g_hash_table_insert(GHashTable	*hash_table,
					               gpointer	 key,
					               gpointer	 value)
{
  GHashNode **node;
  
  g_return_if_fail (hash_table != NULL);
  g_return_if_fail (hash_table->ref_count > 0);
  
  node = g_hash_table_lookup_node (hash_table, key);
  
  if (*node)
  {
    /* do not reset node->key in this place, keeping
     * the old key is the intended behaviour. 
     * g_hash_table_replace() can be used instead.
     */

    /* free the passed key */
    if (hash_table->key_destroy_func)
      hash_table->key_destroy_func (key);
    
    if (hash_table->value_destroy_func)
      hash_table->value_destroy_func ((*node)->value);

    (*node)->value = value;
  }
  else
  {
    *node = g_hash_node_new (key, value);
    hash_table->nnodes++;
    G_HASH_TABLE_RESIZE (hash_table);
  }
}

gboolean g_hash_table_remove(GHashTable	*hash_table,
					                   gconstpointer	 key)
{
  GHashNode **node, *dest;
  
  g_return_val_if_fail (hash_table != NULL, FALSE);
  
  node = g_hash_table_lookup_node (hash_table, key);
  if (*node)
  {
    dest = *node;
    (*node) = dest->next;
    g_hash_node_destroy (dest, 
		   hash_table->key_destroy_func,
		   hash_table->value_destroy_func);
    hash_table->nnodes--;

    G_HASH_TABLE_RESIZE (hash_table);

    return TRUE;
  }

  return FALSE;
}

guint	g_hash_table_size(GHashTable	*hash_table)
{
  g_return_val_if_fail (hash_table != NULL, 0);
  
  return hash_table->nnodes;
}

void g_hash_table_foreach(GHashTable	*hash_table,
					                GHFunc		 func,
					                gpointer	 user_data)
{
  GHashNode *node;
  gint i;
  
  g_return_if_fail (hash_table != NULL);
  g_return_if_fail (func != NULL);
  
  for (i = 0; i < hash_table->size; i++)
    for (node = hash_table->nodes[i]; node; node = node->next)
      (* func) (node->key, node->value, user_data);
}

gpointer g_hash_table_lookup(GHashTable	*hash_table,
					                   gconstpointer	 key)
{
  GHashNode *node;
  
  g_return_val_if_fail (hash_table != NULL, NULL);
  
  node = *g_hash_table_lookup_node (hash_table, key);
  
  return node ? node->value : NULL;
}

guint	g_hash_table_foreach_remove(GHashTable	*hash_table,
					                        GHRFunc	 func,
					                        gpointer	 user_data)
{
  g_return_val_if_fail (hash_table != NULL, 0);
  g_return_val_if_fail (func != NULL, 0);
  
  return g_hash_table_foreach_remove_or_steal (hash_table, func, user_data, TRUE);
}

guint g_str_hash(gconstpointer   v)
{
  /* 31 bit hash function */
  const signed char *p = v;
  guint32 h = *p;

  if (h)
    for (p += 1; *p != '\0'; p++)
      h = (h << 5) - h + *p;

  return h;
}

gboolean g_str_equal(gconstpointer   v1,
                     gconstpointer   v2)
{
  const gchar *string1 = v1;
  const gchar *string2 = v2;
  
  return strcmp (string1, string2) == 0;
}

// GMemChunk

struct _GMemChunk {
  guint alloc_size;           /* the size of an atom */
};

GMemChunk* g_mem_chunk_new(gchar	  *name,
				                   gint	   atom_size,
				                   gulong	   area_size,
				                   gint	   type)
{
  GMemChunk *mem_chunk;
  g_return_val_if_fail (atom_size > 0, NULL);

  UNUSED(name);
  UNUSED(area_size);
  UNUSED(type);

  mem_chunk = malloc(sizeof(GMemChunk));
  mem_chunk->alloc_size = atom_size;
  return mem_chunk;
}

gpointer g_mem_chunk_alloc(GMemChunk *mem_chunk)
{
  g_return_val_if_fail (mem_chunk != NULL, NULL);
  
  return malloc(mem_chunk->alloc_size);
}

void g_mem_chunk_free(GMemChunk *mem_chunk,
				              gpointer   mem)
{
  g_return_if_fail (mem_chunk != NULL);
  
  free(mem);
}

void g_mem_chunk_destroy(GMemChunk *mem_chunk)
{
  g_return_if_fail (mem_chunk != NULL);
  
  free(mem_chunk);
}
