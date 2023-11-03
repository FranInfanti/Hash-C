#include <string.h>
#include <stdlib.h>
#include "hash.h"
#define FACTOR_CARGA_MAXIMO 0.7
#define CANT_MINIMA 3

struct nodo {
	void *valor;
	const char *clave;
	struct nodo *siguiente;
};

struct hash {
	size_t tamanio;
	size_t capacidad;
	struct nodo **array;
};

size_t djb2(const unsigned char *str)
{
        size_t hash = 5381;
        size_t c;

        while ((c = *(str++)))
            hash = ((hash << 5) + hash) + c; 

        return hash;
}

/*
 * Crea un nodo con la clave y el elemento pasado por parametro.
 * 
 * Devuelve un puntero al nodo creado o NULL en caso de error.
 */
struct nodo *crear_nodo(const char *clave, void *elemento)
{
	struct nodo *nuevo = calloc(1, sizeof(struct nodo));
	if (nuevo == NULL)
		return NULL;
	nuevo->clave = clave;
	nuevo->valor = elemento;
	return nuevo;
}

/*
 * Recorre las colisiones hasta llegar al final o encontrar una con la 
 * misma clave que la pasada por parametro.
 * 
 * Devuelve el ultimo nodo de las colisiones o el que coincida con la clave.
 */
struct nodo *recorrer_colisiones(struct nodo *actual, const char *clave)
{
	if (actual == NULL)
		return NULL;
	if (actual->siguiente == NULL) 
		return actual; 
	if (strcmp(actual->clave, clave) == 0) 
		return actual;
	return recorrer_colisiones(actual->siguiente, clave);
}

/*
 * 
 */
bool copiar_hash(const char *clave, void *valor, void *_hash)
{
	struct hash *hash = _hash;
	void *aux = hash_insertar(hash, clave, valor, NULL);
	if (aux == NULL)
		return false;
	hash = aux;
	return true;
}

/*
 * 
 */
struct hash *rehash(struct hash *hash)
{
	struct hash *nuevo = hash_crear(hash->tamanio << 1);
	if (nuevo == NULL)
		return NULL;
	
	hash_con_cada_clave(hash, copiar_hash, (void*)nuevo);

	hash_destruir(hash);
	return nuevo;
}

/*
 * Busca el nodo cuya clave coincida con la pasada por parametro.
 * 
 * Devuelve el nodo que cumpla la condicion o NULL si no lo encuentra.
 */
void *buscar_clave(struct nodo *actual, const char *clave)
{
	if (actual == NULL)
		return NULL;
	if (strcmp(actual->clave, clave) == 0)
		return actual->valor;
	return buscar_clave(actual->siguiente, clave);
}

/* 
 * Recorre todos los nodos de una rama de colisiones hasta que la funcion f
 * devuelva false.
 * 
 * Devuelve la cantidad de elementos a los cuales se le aplico la funcion. 
 */
size_t recorrer_nodos(struct nodo *actual, bool (*f)(const char *clave, void *valor, void *aux), void *aux)
{
	if (actual == NULL)
		return 0;
	if (!f(actual->clave, actual->valor, aux))
		return 1;
	return 1 + recorrer_nodos(actual->siguiente, f, aux);
}

/*
 * Recorre las colisiones de una posicion de la tabla y libera 
 * la memoria de cada nodo.
 */
void liberar_nodos(struct nodo *actual, void (*destructor)(void *))
{
	if (actual == NULL)
		return;
	
	if (destructor != NULL)
		destructor(actual->valor);

	struct nodo *aux = actual->siguiente;
	free(actual);
	liberar_nodos(aux, destructor);
}


hash_t *hash_crear(size_t capacidad)
{	
	struct hash *hash = calloc(1, sizeof(struct hash));
	if (hash == NULL)
		return NULL;

	hash->tamanio = capacidad < CANT_MINIMA ? CANT_MINIMA : capacidad;

	hash->array = calloc(1, hash->tamanio * sizeof(struct nodo*));
	if (hash->array == NULL) {
		free(hash);
		return NULL;
	}

	return hash;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (hash == NULL || clave == NULL)
		return NULL; 

	bool repetido = false;
	size_t posicion = djb2((unsigned char*)clave) % hash->tamanio; 

	struct nodo *nuevo = crear_nodo(clave, elemento);
	if (nuevo == NULL)
		return NULL;

	struct nodo *ultimo = recorrer_colisiones(hash->array[posicion], clave);
    	void *removido = NULL;
    
    	if (ultimo == NULL)
      		hash->array[posicion] = nuevo;
    	else if (strcmp(ultimo->clave, clave) != 0)
      		ultimo->siguiente = nuevo;
    	else {
      		removido = ultimo->valor;
      		ultimo->valor = elemento;
      		free(nuevo);
      		repetido = true;
    	}
	
	if (anterior != NULL)
		*anterior = removido; 
	
	if (!repetido)
		hash->capacidad++;
	
	float factor_carga = (float)hash->capacidad / (float)hash->tamanio;
	if (factor_carga >= FACTOR_CARGA_MAXIMO) {
		void *aux = rehash(hash);
		hash = aux == NULL ? hash : aux;
	}
				
	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	return NULL;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (hash == NULL || clave == NULL)
		return NULL;
	
	size_t posicion = djb2((unsigned char*)clave) % hash->tamanio;
	return buscar_clave(hash->array[posicion], clave);
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	return !!hash_obtener(hash, clave);
}

size_t hash_cantidad(hash_t *hash)
{
	return hash == NULL ? 0 : hash->capacidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (hash == NULL)
		return;

	if (hash->capacidad != 0) {
		for (size_t i = 0; i < hash->tamanio; i++) 
			liberar_nodos(hash->array[i], destructor);
	}
	free(hash->array);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash, bool (*f)(const char *clave, void *valor, void *aux), void *aux)
{
	if (hash == NULL || f == NULL)
		return 0;

	size_t cantidad = 0;

	for (size_t i = 0; i < hash->tamanio; i++) 
		cantidad += recorrer_nodos(hash->array[i], f, aux);

	return cantidad;
}
