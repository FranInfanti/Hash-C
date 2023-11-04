#include <string.h>
#include <stdlib.h>
#include "hash.h"
#define FACTOR_CARGA_MAXIMO 0.7
#define CANT_MINIMA 3

typedef struct nodo {
	void *valor;
	char *clave;
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
 * Crea un nodo copiando el valor de la clave 
 * y el elemento pasado por parametro.
 * 
 * Devuelve un puntero al nodo creado o NULL en caso de error.
 */
nodo_t *crear_nodo(const char *clave, void *elemento)
{
	nodo_t *nuevo = calloc(1, sizeof(struct nodo));
	if (nuevo == NULL)
		return NULL;

	nuevo->clave = calloc(1, (strlen(clave) + 1) * sizeof(char));
	if (nuevo->clave == NULL)
		return NULL;

	memcpy(nuevo->clave, clave, (strlen(clave) + 1));
	nuevo->valor = elemento;
	return nuevo;
}

/*
 * Busca el nodo cuya clave coincida con la clave 
 * pasada por parametro.
 * 
 * Devuelve el nodo que cumple la condicion anterior o 
 * NULL si no lo encuentra.
 */
nodo_t *buscar_clave(nodo_t *actual, const char *clave)
{
	if (actual == NULL)
		return NULL;
	if (strcmp(actual->clave, clave) == 0)
		return actual;
	return buscar_clave(actual->siguiente, clave);
}


/*
 * Recibe el hash, la posicion donde se debe insertar y un nuevo nodo que contiene 
 * la informacion del elemento a insertar.
 * 
 * Inserta el nuevo nodo en la tabla o intercambia el valor si la clave coincide con 
 * alguna del arbol.
 * 
 * Devuelve un puntero al elemento que se intercambio, si es que lo hizo, caso contrario 
 * se devuelve NULL.
 */
void *insertar_colision(struct hash *hash, unsigned long posicion, nodo_t **nuevo)
{
	void *swap = NULL;
	nodo_t *existe = buscar_clave(hash->array[posicion], (*nuevo)->clave);
	if (existe == NULL) {
		(*nuevo)->siguiente = hash->array[posicion];
		hash->array[posicion] = *nuevo;
	} else {
		swap = existe->valor;
		existe->valor = (*nuevo)->valor;
		free((*nuevo)->clave);
		free(*nuevo);
		*nuevo = NULL;
	}
	return swap;
}



/*
 * Recorre las colisiones hasta encontrar el nodo anterior al 
 * nodo que coincide con la clave pasada por parametro.
 *
 * Devuelve el anterior a eliminar o NULL si no existe el 
 * nodo que coincide con la clave pasada por parametro
 */
nodo_t *anterior_a_eliminar(nodo_t *actual, const char *clave)
{
	if (actual->siguiente == NULL)
		return NULL;
	if (strcmp(actual->siguiente->clave, clave) == 0)
		return actual;
	return anterior_a_eliminar(actual->siguiente, clave);
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
	free(actual->clave);
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
	void *swap = NULL;
	bool repetido = false;

	if (hash->array[posicion] == NULL) {
		hash->array[posicion] = crear_nodo(clave, elemento);
		if (hash->array[posicion] == NULL)
			return NULL;
	} else {
		nodo_t *existe = buscar_clave(hash->array[posicion], clave);
		if (existe == NULL) {
			void *aux = hash->array[posicion];
			hash->array[posicion] = crear_nodo(clave, elemento);
			if (hash->array[posicion] == NULL) {
				hash->array[posicion] = aux;
				return NULL;
			}
			hash->array[posicion]->siguiente = aux;
		} else {
			swap = existe->valor;
			existe->valor = elemento;
			repetido = true;
		}
	}
	
	if (anterior != NULL)
		*anterior = swap;
	if (!repetido)
		hash->capacidad++;

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
		anterior = anterior_a_eliminar(hash->array[posicion], clave);
		if (anterior == NULL)
			return NULL;
		eliminado = anterior->siguiente;
		anterior->siguiente = eliminado->siguiente;
	}

	valor = eliminado->valor;
	free(eliminado->clave);
	free(eliminado);
	hash->capacidad--;
	return valor;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (hash == NULL || clave == NULL)
		return NULL;

	size_t posicion = djb2((unsigned char *)clave) % hash->tamanio;
	nodo_t *buscado = buscar_clave(hash->array[posicion], clave);

	return buscado == NULL ? NULL : buscado->valor;
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




bool copiar_hash(const char *clave, void *valor, void *_hash)
{
	struct hash *hash = _hash;
	void *aux = hash_insertar(hash, clave, valor, NULL);
	if (aux == NULL)
		return false;
	hash = aux;
	return true;
}

struct hash *rehash(struct hash *hash)
{
	struct hash *nuevo = hash_crear(hash->tamanio << 1);
	if (nuevo == NULL)
		return NULL;

	for (size_t i = 0; i < hash->tamanio; i++) {
		nodo_t *actual = hash->array[i];
		while (actual != NULL) {
			size_t posicion = djb2((unsigned char *)actual->clave) % hash->tamanio; 
			insertar_colision(nuevo, posicion, &actual);
			actual = actual->siguiente;
		}
	}
	//hash_con_cada_clave(hash, copiar_hash, (void*)nuevo);
	nuevo->capacidad = hash->capacidad;
	free(hash->array);
	free(hash);
	return nuevo;
}
