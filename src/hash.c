#include <string.h>
#include <stdlib.h>
#include "hash.h"
#define FACTOR_CARGA_MAXIMO 0.7
#define CANT_MINIMA 3

typedef struct nodo {
	void *valor;
	const char *clave;
	struct nodo *siguiente;
} nodo_t;

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
nodo_t *crear_nodo(const char *clave, void *elemento)
{
	nodo_t *nuevo = calloc(1, sizeof(struct nodo));
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
nodo_t *recorrer_colision(nodo_t *actual, const char *clave)
{
	if (actual == NULL)
		return NULL;
	if (actual->siguiente == NULL || strcmp(actual->clave, clave) == 0)
		return actual;

	return recorrer_colision(actual->siguiente, clave);
}

/*
 * Inserta el nuevo nodo en la tabla de hash, o actualiza el valor
 * del nodo cuya clave sea igual que la del nodo nuevo.
 * 
 * Devuelve el valor anterior que tenia el nodo, si es que se intercambio, sino
 * devuelve NULL.
 */
void *insertar_nodo(struct hash *hash, size_t posicion, nodo_t *nuevo)
{
	bool repetido = false;

	nodo_t *ultimo = recorrer_colision(hash->array[posicion], nuevo->clave);
	void *removido = NULL;

	if (ultimo == NULL)
		hash->array[posicion] = nuevo;

	else if (strcmp(ultimo->clave, nuevo->clave) != 0)
		ultimo->siguiente = nuevo;

	else {
		removido = ultimo->valor;
		ultimo->valor = nuevo->valor;
		repetido = true;
		free(nuevo);
	}

	if (!repetido)
		hash->capacidad++;

	return removido;
}

/*
 * 
 */
nodo_t *buscar_eliminado(nodo_t *actual, const char *clave)
{
	if (actual->siguiente == NULL)
		return NULL;
	if (strcmp(actual->siguiente->clave, clave) == 0)
		return actual;
	return buscar_eliminado(actual->siguiente, clave);
}

/*
 * Busca el nodo cuya clave coincida con la pasada por parametro.
 * 
 * Devuelve el valor que almecena el nodo que cumpla la condicion o NULL si no lo encuentra.
 */
void *buscar_clave(nodo_t *actual, const char *clave)
{
	if (actual == NULL)
		return NULL;
	if (strcmp(actual->clave, clave) == 0)
		return actual->valor;
	return buscar_clave(actual->siguiente, clave);
}

/*
 * Recorre las colisiones de una posicion de la tabla y libera 
 * la memoria de cada nodo.
 */
void liberar_nodos(nodo_t *actual, void (*destructor)(void *))
{
	if (actual == NULL)
		return;

	if (destructor != NULL)
		destructor(actual->valor);

	void *aux = actual->siguiente;
	free(actual);
	liberar_nodos(aux, destructor);
}

/* 
 * Recorre todos los nodos de una rama de colisiones hasta que la funcion f
 * devuelva false.
 * 
 * Devuelve la cantidad de elementos a los cuales se le aplico la funcion. 
 */
size_t recorrer_nodos(nodo_t *actual,
		      bool (*f)(const char *clave, void *valor, void *aux),
		      void *aux, bool *sigo)
{
	if (actual == NULL)
		return 0;
	if (!f(actual->clave, actual->valor, aux)) {
		*sigo = false;
		return 1;
	}
	return 1 + recorrer_nodos(actual->siguiente, f, aux, sigo);
}


hash_t *hash_crear(size_t capacidad)
{
	struct hash *hash = calloc(1, sizeof(struct hash));
	if (hash == NULL)
		return NULL;

	hash->tamanio = capacidad < CANT_MINIMA ? CANT_MINIMA : capacidad;

	hash->array = calloc(1, hash->tamanio * sizeof(struct nodo *));
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

	size_t posicion = djb2((unsigned char *)clave) % hash->tamanio;

	nodo_t *nuevo = crear_nodo(clave, elemento);
	if (nuevo == NULL)
		return NULL;

	void *removido = insertar_nodo(hash, posicion, nuevo);

	if (anterior != NULL)
		*anterior = removido;

	/* Falta rehash */

	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (hash == NULL || clave == NULL)
		return NULL;

	size_t posicion = djb2((unsigned char *)clave) % hash->tamanio;
	if (hash->array[posicion] == NULL)
		return NULL;

	void *valor = NULL;
	nodo_t *eliminado = NULL;
	nodo_t *anterior = NULL;


	if (strcmp(hash->array[posicion]->clave, clave) == 0) {
		eliminado = hash->array[posicion];
		hash->array[posicion] = eliminado->siguiente;
	} else {
		anterior = buscar_eliminado(hash->array[posicion], clave);
		if (anterior == NULL)
			return NULL;
		eliminado = anterior->siguiente;
		anterior->siguiente = eliminado->siguiente;
	}

	valor = eliminado->valor;
	free(eliminado);
	hash->capacidad--;

	return valor;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (hash == NULL || clave == NULL)
		return NULL;

	size_t posicion = djb2((unsigned char *)clave) % hash->tamanio;
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

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	if (hash == NULL || f == NULL)
		return 0;

	size_t cantidad = 0;
	bool sigo = true;

	for (size_t i = 0; i < hash->tamanio && sigo; i++)
		cantidad += recorrer_nodos(hash->array[i], f, aux, &sigo);

	return cantidad;
}
