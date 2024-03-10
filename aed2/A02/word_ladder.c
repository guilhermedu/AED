//
// AED, November 2022 (Tomás Oliveira e Silva)
//
// Second practical assignement (speed run)
//
// Place your student numbers and names here
//   N.Mec. 110555  Name: Joao
//
// Do as much as you can
//   1) MANDATORY: complete the hash table code
//      *) hash_table_create
//      *) hash_table_grow
//      *) hash_table_free
//      *) find_word
//      +) add code to get some statistical data about the hash table
//   2) HIGHLY RECOMMENDED: build the graph (including union-find data) -- use the similar_words function...
//      *) find_representative
//      *) add_edge
//   3) RECOMMENDED: implement breadth-first search in the graph
//      *) breadh_first_search
//   4) RECOMMENDED: list all words belonginh to a connected component
//      *) breadh_first_search
//      *) list_connected_component
//   5) RECOMMENDED: find the shortest path between to words
//      *) breadh_first_search
//      *) path_finder
//      *) test the smallest path from bem to mal
//         [ 0] bem
//         [ 1] tem
//         [ 2] teu
//         [ 3] meu
//         [ 4] mau
//         [ 5] mal
//      *) find other interesting word ladders
//   6) OPTIONAL: compute the diameter of a connected component and list the longest word chain
//      *) breadh_first_search
//      *) connected_component_diameter
//   7) OPTIONAL: print some statistics about the graph
//      *) graph_info
//   8) OPTIONAL: test for memory leaks
//





#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
// static configuration 
//

#define _max_word_size_  32


//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s  adjacency_node_t;
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s      hash_table_t;

struct adjacency_node_s
{
  adjacency_node_t *next;            // link to the next adjacency list node
  hash_table_node_t *vertex;         // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  char word[_max_word_size_];        // the word
  hash_table_node_t *next;           // next hash table linked list node
  // the vertex data
  adjacency_node_t *head;            // head of the linked list of adjancency edges
  int visited;                       // visited status (while not in use, keep it at 0)
  hash_table_node_t *previous;       // breadth-first search parent
  // the union find data
  hash_table_node_t *representative; // the representative of the connected component this vertex belongs to
  int number_of_vertices;            // number of vertices of the conected component (only correct for the representative of each connected component)
  int number_of_edges;               // number of edges of the conected component (only correct for the representative of each connected component)
};

struct hash_table_s
{
  unsigned int hash_table_size;      // the size of the hash table array (tamanho)
  unsigned int number_of_entries;    // the number of entries in the hash table (numero de entradas de palavras) inicia a 0
  unsigned int number_of_edges;      // number of edges (for information purposes only)
  hash_table_node_t **heads;         // the heads of the linked lists  (pronteiro para um array de ponteiros para inicio de cadeias de palavras(nó )) 
};
struct queue_s
{
  hash_table_node_t *node;
  struct queue_s *next;
};

//
// allocation and deallocation of linked list nodes (done)
//

static adjacency_node_t *allocate_adjacency_node(void)
{
  adjacency_node_t *node;

  node = (adjacency_node_t *)malloc(sizeof(adjacency_node_t)); 
  if(node == NULL) 
  {
    fprintf(stderr,"allocate_adjacency_node: out of memory\n");
    exit(1);
  }
  return node; // retorna o no
}

static void free_adjacency_node(adjacency_node_t *node)
{
  free(node);
}

static hash_table_node_t *allocate_hash_table_node(void)
{
  hash_table_node_t *node;

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_hash_table_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}


//
// hash table stuff (mostly to be done)
//

unsigned int crc32(const char *str)
{
  static unsigned int table[256];
  unsigned int crc;

  if(table[1] == 0u) // do we need to initialize the table[] array?
  {
    unsigned int i,j;

    for(i = 0u;i < 256u;i++)
      for(table[i] = i,j = 0u;j < 8u;j++)
        if(table[i] & 1u)
          table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
        else
          table[i] >>= 1;
  }
  crc = 0xAED02022u; // initial value (chosen arbitrarily)
  while(*str != '\0')
    crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
  return crc;
}

static hash_table_t *hash_table_create(void)
{
  hash_table_t *hash_table;
  unsigned int i;

  hash_table = (hash_table_t *)malloc(sizeof(hash_table_t)); // aloca memoria para a tabela hash
  if(hash_table == NULL) // se nao conseguir alocar memoria
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }
  // Inicializar os atributos da tabela hash
  hash_table->hash_table_size = 200u; // Define o tamanho da tabela hash
  hash_table->number_of_entries = 0u; // numero de entradas de palavras
  hash_table->number_of_edges = 0u; // numero de arestas
  hash_table->heads = (hash_table_node_t **)malloc(hash_table->hash_table_size * sizeof(hash_table_node_t *)); // aloca memoria para o array de ponteiros para inicio de cadeias de palavras(nó )
  if(hash_table->heads == NULL) // se nao conseguir alocar memoria
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }
  for(i = 0u;i < hash_table->hash_table_size;i++) // inicializa a tabela hash a NULL
    hash_table->heads[i] = NULL;
  
  return hash_table;
}

static void hash_table_grow(hash_table_t *hash_table)
{
  hash_table_node_t **old_heads, **new_heads, *node, *next;
  unsigned int old_size, i;

  // save a pointer to the old array of linked list heads and its size
  old_heads = hash_table->heads;
  old_size = hash_table->hash_table_size;

  // create a new hash table with a larger size
  hash_table->hash_table_size *= 2u;
  new_heads = (hash_table_node_t **)malloc(hash_table->hash_table_size * sizeof(hash_table_node_t *));
  // check for out of memory
  for (i = 0u; i < hash_table->hash_table_size; i++)
    new_heads[i] = NULL;

  if (new_heads == NULL)
  {
    fprintf(stderr, "hash_table_grow: out of memory");
    exit(1);
  }

  // run the hash function for old values with new size

  for (i = 0u; i < old_size; i++)
  {
    node = old_heads[i];
    while (node != NULL)
    {

      next = node->next;

      size_t index = crc32(node->word) % hash_table->hash_table_size;
      node->next = new_heads[index];
      new_heads[index] = node;

      node = next;
    }
  }
  
  free(old_heads);
  hash_table->heads = new_heads;
  
  
}

static void hash_table_free(hash_table_t *hash_table)
{
  hash_table_node_t *node;
  hash_table_node_t *temp;
  adjacency_node_t *adj_node;
  adjacency_node_t *temp_adj;
  unsigned int i;

  for (i = 0; i < hash_table->hash_table_size; i++)
  {
    node = hash_table->heads[i];
    while (node != NULL)
    {
      temp = node;
      adj_node = node->head;
      while (adj_node != NULL)
      {
        temp_adj = adj_node;
        adj_node = adj_node->next;
        free(temp_adj);
      }
      node = node->next;
      free(temp);
    }
  }

  // Free the memory used by the array of linked list heads
  free(hash_table->heads);

  // Free the memory used by the hash table
  free(hash_table);

}

static hash_table_node_t *find_word(hash_table_t *hash_table,const char *word,int insert_if_not_found)
{
  hash_table_node_t *node; // nó da tabela hash
  unsigned int i;

  i = crc32(word) % hash_table->hash_table_size; // calcula o indice da tabela hash

  node = hash_table->heads[i]; // aponta para o inicio da cadeia de palavras
  while(node != NULL) // percorre a cadeia de palavras (ate encontrar a palavra ou chegar ao fim da cadeia)
  {
    if(strcmp(node->word,word) == 0) // se encontrar a palavra
      return node; // retorna o nó
    node = node->next; // aponta para o proximo nó
  }

  if(insert_if_not_found && strlen(word)<_max_word_size_) // se nao encontrar a palavra e nao for para a inserir na tabela
  {

  
  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t)); // aloca memoria para o nó
  strcpy(node->word,word); // copia a palavra para o nó
  node->representative = node; // aponta para si proprio
  node->next = hash_table->heads[i]; // aponta para o inicio da cadeia de palavras
  node->previous = NULL; // aponta para NULL
  node->number_of_edges = 0; // inicializa o numero de arestas a 0
  node->number_of_vertices = 1; // inicializa o numero de vertices a 1
  node->visited=0; // inicializa o visited a 0
  node->head =NULL; // inicializa o head a NULL
  hash_table->heads[i] = node; // aponta para o novo nó e assim insere-o no inicio da cadeia de palavras
  hash_table->number_of_entries++; // incrementa o numero de entradas
  if(hash_table->number_of_entries > hash_table->hash_table_size) // se o numero de entradas for maior que o tamanho da tabela hash
    hash_table_grow(hash_table); // aumenta o tamanho da tabela hash
  return node;
  }
  return NULL;

  

}
  



  


//
// add edges to the word ladder graph (mostly do be done)
//

static hash_table_node_t *find_representative(hash_table_node_t *node)
{
  hash_table_node_t *representative,*next_node;

  for(representative = node; representative != representative->representative; representative = representative->representative)
    ;
  for(next_node = node; next_node != representative; next_node = next_node->representative)//o representativo é o vertice que representa cada componente conexo de um grafo à medida que tu vais adicionando edges o representativo vai mudar
  {
    node = next_node->representative;
    next_node->representative = representative;
  }
  return representative;
}

static int find_connected_component_representatives(hash_table_t *hash_table, hash_table_node_t **representatives)
{
    int index = 0;
    for (int i = 0; i < hash_table->hash_table_size; i++)
    {
        hash_table_node_t *vertex = hash_table->heads[i];
        while(vertex != NULL)
        {
            hash_table_node_t *representative = find_representative(vertex);
            if (!representative->visited)
            {
                representatives[index] = representative;
                representative->visited = 1;
                index++;
            }
            vertex = vertex->next;
        }
    }
    for (int i = 0; i < hash_table->hash_table_size; i++)
    {
        hash_table_node_t *vertex = hash_table->heads[i];
        while(vertex != NULL)
        {
            vertex->visited = 0;
            vertex = vertex->next;
        }
    }
    return index;
}

static void add_edge(hash_table_t *hash_table,hash_table_node_t *from,const char *word)
{
    hash_table_node_t *to, *from_representative, *to_representative;
  adjacency_node_t *linkfrom, *linkto;

  from_representative = find_representative(from);
  to = find_word(hash_table, word, 0);

  if (to == NULL || to == from)
    return;

  to_representative = find_representative(to);
  if (from_representative == to_representative)
  {
    from_representative->number_of_vertices++;
  }

  if (from_representative != to_representative)
  {
    if (from_representative->number_of_vertices < to_representative->number_of_vertices)
    {
      from_representative->representative = to_representative;
      to_representative->number_of_vertices += from_representative->number_of_vertices;
      to_representative->number_of_edges += from_representative->number_of_edges;
    }
    else
    {
      to_representative->representative = from_representative;
      from_representative->number_of_vertices += to_representative->number_of_vertices;
      from_representative->number_of_edges += to_representative->number_of_edges;
    }
  }

  linkfrom = allocate_adjacency_node();
  linkto = allocate_adjacency_node();

  if (linkfrom == NULL || linkto == NULL)
  {
    fprintf(stderr, "add_edge: out of memory\n");
    exit(1);
  }

  linkfrom->vertex = to;
  linkfrom->next = from->head;
  from->head = linkfrom;

  linkto->vertex = from;
  linkto->next = to->head;
  to->head = linkto;

  from_representative->number_of_edges++;
  to_representative->number_of_edges++;
  hash_table->number_of_edges++;
  return;
  
}

static void hash_statistical(hash_table_t *hash_table)
{
  printf("\nHash table size: %u\n", hash_table->hash_table_size);
  printf("Número of entries %u\n", hash_table->number_of_entries);
  printf("Tamanho médio das listas: %f\n",(float)hash_table->number_of_entries / hash_table->hash_table_size);
  printf("number of edges %u\n", hash_table->number_of_edges);
  

  hash_table_node_t *node;
  unsigned int MaxHead =0;
  unsigned int MinHead = _max_word_size_;
  for(unsigned int i=0u;i<hash_table->hash_table_size;i++)
  {
    unsigned int sizeH=0;

    for(node = hash_table->heads[i]; node != NULL; node = node->next)
    {
      sizeH++;
    }

    if(sizeH>MaxHead)
    {
      MaxHead=sizeH;
    }
    if(sizeH<MinHead && sizeH!=0)
    {
      MinHead=sizeH;
    }

  }
  printf("Maximum list length: %u\n", MaxHead);
  printf("Minimum list length: %u\n", MinHead);
 
  printf("\n");
}

//
// generates a list of similar words and calls the function add_edge for each one (done)
//
// man utf8 for details on the uft8 encoding
//

static void break_utf8_string(const char *word,int *individual_characters)
{
  int byte0,byte1;

  while(*word != '\0')
  {
    byte0 = (int)(*(word++)) & 0xFF;
    if(byte0 < 0x80)
      *(individual_characters++) = byte0; // plain ASCII character
    else
    {
      byte1 = (int)(*(word++)) & 0xFF;
      if((byte0 & 0b11100000) != 0b11000000 || (byte1 & 0b11000000) != 0b10000000)
      {
        fprintf(stderr,"break_utf8_string: unexpected UFT-8 character\n");
        exit(1);
      }
      *(individual_characters++) = ((byte0 & 0b00011111) << 6) | (byte1 & 0b00111111); // utf8 -> unicode
    }
  }
  *individual_characters = 0; // mark the end!
}

static void make_utf8_string(const int *individual_characters,char word[_max_word_size_])
{
  int code;

  while(*individual_characters != 0)
  {
    code = *(individual_characters++);
    if(code < 0x80)
      *(word++) = (char)code;
    else if(code < (1 << 11))
    { // unicode -> utf8
      *(word++) = 0b11000000 | (code >> 6);
      *(word++) = 0b10000000 | (code & 0b00111111);
    }
    else
    {
      fprintf(stderr,"make_utf8_string: unexpected UFT-8 character\n");
      exit(1);
    }
  }
  *word = '\0';  // mark the end
}

static void similar_words(hash_table_t *hash_table,hash_table_node_t *from)
{
  static const int valid_characters[] =
  { // unicode!
    0x2D,                                                                       // -
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,           // A B C D E F G H I J K L M
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,           // N O P Q R S T U V W X Y Z
    0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,           // a b c d e f g h i j k l m
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,           // n o p q r s t u v w x y z
    0xC1,0xC2,0xC9,0xCD,0xD3,0xDA,                                              // Á Â É Í Ó Ú
    0xE0,0xE1,0xE2,0xE3,0xE7,0xE8,0xE9,0xEA,0xED,0xEE,0xF3,0xF4,0xF5,0xFA,0xFC, // à á â ã ç è é ê í î ó ô õ ú ü
    0
  };
  int i,j,k,individual_characters[_max_word_size_];
  char new_word[2 * _max_word_size_];

  break_utf8_string(from->word,individual_characters);
  for(i = 0;individual_characters[i] != 0;i++)
  {
    k = individual_characters[i];
    for(j = 0;valid_characters[j] != 0;j++)
    {
      individual_characters[i] = valid_characters[j];
      make_utf8_string(individual_characters,new_word);
      // avoid duplicate cases
      if(strcmp(new_word,from->word) > 0)
        add_edge(hash_table,from,new_word);
    }
    individual_characters[i] = k;
  }
}


//
// breadth-first search (to be done)
//
// returns the number of vertices visited; if the last one is goal, following the previous links gives the shortest path between goal and origin
//

static int breadh_first_search(int maximum_number_of_vertices,hash_table_node_t **list_of_vertices,hash_table_node_t *origin,hash_table_node_t *goal)
{
  int i,j;
  hash_table_node_t *node;

  // initialize the queue
  i = 0;
  j = 1;
  list_of_vertices[0] = origin;
  origin->previous = NULL;
  origin->visited = 1;
  int k = 0;
  




  // breadth-first search
  while(i != j)
  {
    adjacency_node_t *node=list_of_vertices[i++]->head;
    if(k==1)
    {
      break;
    }
    while(node!=NULL)
    {
      if(node->vertex->visited==0)
      {
        node->vertex->visited = 1;
        node->vertex->previous = list_of_vertices[i-1];
        list_of_vertices[j++] = node->vertex;
        if(node->vertex==goal)
        {
          k=1;
          break;
        }
      }
      node=node->next;
    }
  
  }
  for(i = 0;i < j;i++)
  {
    list_of_vertices[i]->visited = 0;
  }
  return j;
}



//
// list all vertices belonging to a connected component (complete this)
//

static void list_connected_component(hash_table_t *hash_table,const char *word) // not sure if this is right
{
    hash_table_node_t *node, *rep;
  int i, n, maximum_number_of_vertices;
  hash_table_node_t **list_of_vertices;

  node = find_word(hash_table,word,0);
  
  if(node == NULL)
  {
    printf("list_connected_component: word not found\n");
    return;
  }

  rep = find_representative(node);
  maximum_number_of_vertices = rep->number_of_vertices;
  list_of_vertices = (hash_table_node_t **)malloc(maximum_number_of_vertices * sizeof(hash_table_node_t *));
  
  if(list_of_vertices == NULL)
  {
    fprintf(stderr,"list_connected_component: out of memory\n");
    exit(1);
  }
  
  n = breadh_first_search(maximum_number_of_vertices,list_of_vertices,node,NULL);
  int count = 0;
  for(i = 0;i < n;i++){
    printf("%d: %s\n",count,list_of_vertices[i]->word);
    count++;
  }
  free(list_of_vertices);
  //free(hash_table);
} 


//
// compute the diameter of a connected component (optional)
//

static int largest_diameter; // registar maior diametro
static hash_table_node_t **largest_diameter_example; // registar o exemplo do maior diametro

static int connected_component_diameter(hash_table_node_t *node)
{
  //é o caminho mais pequeno maior entre duas palavras que podes fazer num componente conexo 
  int diameter; // diametro
  int i; 
  
  
  //if(node->visited) // se a palavra ja tiver sido visitada
  //{
  //  fprintf(stderr,"connected_component_diameter: word already visited\n");
  //  exit(1);
  //}
  //node->visited = 1; // marca a palavra como visitada

  //if(node->number_of_edges == 0) // se a palavra nao tiver ligacoes
  //{
  //  diameter; // o diametro da palavra e 0
  //  diameter_example = NULL; // nao existe exemplo 
  //  number_of_vertices_visited = 1; // a palavra foi visitada
  //  list_of_vertices_visited[0] = node; // a palavra foi visitada
  //  return 0;
  //}

  
  
  //if(node->diameter > largest_diameter) // se o diametro da palavra atual for maior que o maior diametro
  //{
  //  largest_diameter = node->diameter; // o maior diametro passa a ser o diametro da palavra atual
  //  largest_diameter_example = node->list_of_vertices_visited; // o exemplo do maior diametro passa a ser a lista de vertices visitados da palavra atual
  //}

}


//
// find the shortest path from a given word to another given word (to be done)
//

static void path_finder(hash_table_t *hash_table,const char *from_word,const char *to_word)
{//é achar o caminho + pequeno 
//entre uma palavra e outra
//sabendo que tão no mesmo componente conexo
//tambêm conhecido como
//fazer breadth first search
  hash_table_node_t *fromNode, *fromRep, *toNode, *toRep;
  fromNode = find_word(hash_table, from_word, 0);
  toNode = find_word(hash_table, to_word, 0);
  
  if(fromNode == NULL || toNode == NULL){
    printf("One of the words is not in the dictionary\n");
    return;
  }

  fromRep = find_representative(fromNode);
  toRep = find_representative(toNode);

  if(fromRep != toRep){
    printf("The words are not in the same connected component, so there's no path between them.\n");
    return;
  }

  hash_table_node_t **list_of_vertices = malloc(sizeof(hash_table_node_t *) * fromRep->number_of_vertices);
  
  if (list_of_vertices == NULL)
  {
    fprintf(stderr, "path_finder: malloc failed\n");
    exit(1);
  }

  int goalIndex = breadh_first_search(fromRep->number_of_vertices, list_of_vertices, toNode, fromNode);

  hash_table_node_t *p = list_of_vertices[goalIndex-1];
  int count = 0;
  while (p != NULL)
  {
    printf("%d: %s \n",count, p->word);
    count++;
    p = p->previous;
  }
  
  free(list_of_vertices);
}
  
  
  

  



//
// some graph information (optional)
//

static void graph_info(hash_table_t *hash_table)
{
  //number of nodes
  // number of edges
  //number of connected components
  //size of the largest connected component
  //size of the smallest connected component
  //average size of the connected components
  printf("Número de vértices: %d\n", hash_table->number_of_entries);
  printf("Número de arestas: %d\n\n", hash_table->number_of_edges);

  hash_table_node_t **representatives = malloc(sizeof(hash_table_node_t *) * hash_table->hash_table_size);
     
  int nrRepresentative = find_connected_component_representatives(hash_table, representatives);
  printf("Número de componentes conexas: %d\n\n", nrRepresentative);
   

}


//
// main program
//

int main(int argc,char **argv)
{
  char word[100],from[100],to[100];
  hash_table_t *hash_table;
  hash_table_node_t *node;
  unsigned int i;
  int command;
  FILE *fp;
  printf("Word Ladder Solver\n");
  // initialize hash table
  hash_table = hash_table_create();
  // read words
  fp = fopen((argc < 2) ? "wordlist-four-letters.txt" : argv[1],"rb");
  if(fp == NULL)
  {
    fprintf(stderr,"main: unable to open the words file\n");
    exit(1);
  }
  
    while(fscanf(fp,"%99s",word) == 1)
      (void)find_word(hash_table,word,1);
    fclose(fp);
    // find all similar words

  for(i = 0u;i < hash_table->hash_table_size;i++)
  { 
    for(node = hash_table->heads[i];node != NULL;node = node->next)
    {
      similar_words(hash_table, node);
    }
  }
  

  // ask what to do
  for(;;)
  {
    fprintf(stderr,"Your wish is my command:\n");
    fprintf(stderr,"  1 WORD       (list the connected component WORD belongs to)\n");
    fprintf(stderr,"  2 FROM TO    (list the shortest path from FROM to TO)\n");
    fprintf(stderr,"  3            (terminate)\n");
    fprintf(stderr,"  4           (graph info)\n");
    fprintf(stderr,"  5           (statistical)\n ");
    if(scanf("%99s",word) != 1)
      break;
    command = atoi(word);
    if(command == 1)
    {
      if(scanf("%99s",word) != 1)
        break;
      list_connected_component(hash_table,word);
    }
    else if(command == 2)
    {
      if(scanf("%99s",from) != 1)
        break;
      if(scanf("%99s",to) != 1)
        break;
      path_finder(hash_table,from,to);
    }
    else if(command == 3)
      break;
    else if(command == 4)
      graph_info(hash_table);
    else if(command == 5)
      hash_statistical(hash_table);
  }
  // clean up
  hash_table_free(hash_table);

  
  return 0;
}