#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "hash.h"
#define FACTOR_CARGA_MAXIMO 0.7
#define CANT_MINIMA 3

struct nodo {
	void *valor;
	char *clave;
	struct nodo *siguiente;
};

struct hash {
	size_t tamanio;
	size_t capacidad;
	struct nodo **array;
};

size_t djb2(void *data)
{
	const unsigned char *str = data;
	size_t hash = 5381;
	int c;

	while ((c = *(str++)))
		hash = ((hash << 5) + hash) + (size_t)c;

	return hash;
}

/*
 * Crea un nodo copiando el valor de la clave 
 * y el elemento pasado por parametro.
 * 
 * Devuelve un puntero al nodo creado o NULL en caso de error.
 */
struct nodo *crear_nodo(const char *clave, void *elemento)
{
	struct nodo *nuevo = calloc(1, sizeof(struct nodo));
	if (!nuevo)
		return NULL;

	size_t tamanio = strlen(clave) + 1;
	nuevo->clave = calloc(1, tamanio * sizeof(char));
	if (!nuevo->clave) {
		free(nuevo);
		return NULL;
	}
	memcpy(nuevo->clave, clave, tamanio);
	nuevo->valor = elemento;
	return nuevo;
}

/*
 * Inserta en la tabla de hash, en la posicion recibida por parametro, el
 * nuevo nodo.
 * 
 * Devuelve true si se pudo insertar el elemenento correctamente
 * o false en caso contrario.
 */
bool insertar_nodo(struct hash *hash, size_t posicion, struct nodo *nuevo)
{
	if (!nuevo)
		return false;

	void *aux = hash->array[posicion];
	nuevo->siguiente = aux;
	hash->array[posicion] = nuevo;
	hash->capacidad++;
	return true;
}

/*
 * Busca el nodo cuya clave coincida con la clave 
 * pasada por parametro.
 * 
 * Devuelve el nodo que cumple la condicion anterior o 
 * NULL si no lo encuentra.
 */
struct nodo *buscar_clave(struct nodo *actual, const char *clave)
{
	if (actual == NULL)
		return NULL;
	if (strcmp(actual->clave, clave) == 0)
		return actual;
	return buscar_clave(actual->siguiente, clave);
}

/*
 * Recorre las colisiones hasta encontrar el nodo anterior 
 * al nodo que coincida con la clave pasada por parametro.
 * 
 * Devuelve un puntero al nodo anterior, o si no existe, devuelve NULL.
 */
struct nodo *anterior_a_eliminar(struct nodo *actual, const char *clave)
{
	if (actual->siguiente == NULL)
		return NULL;
	if (strcmp(actual->siguiente->clave, clave) == 0)
		return actual;
	return anterior_a_eliminar(actual->siguiente, clave);
}

/*
 * Elimina el nodo cuya clave coincida con la pasada por parametro, en
 * caso de que no exista, no hace nada.
 * 
 * Devuelve el valor del nodo que se elimino o NULL si no existe. 
 */
void *quitar_nodo(struct hash *hash, size_t posicion, const char *clave)
{
	struct nodo *eliminar = NULL;
	struct nodo *anterior = NULL;
	void *removido = NULL;

	if (strcmp(hash->array[posicion]->clave, clave) == 0) {
		eliminar = hash->array[posicion];
		hash->array[posicion] = eliminar->siguiente;
	} else {
		anterior = anterior_a_eliminar(hash->array[posicion], clave);
		if (!anterior)
			return NULL;
		eliminar = anterior->siguiente;
		anterior->siguiente = eliminar->siguiente;
	}
	removido = eliminar->valor;
	free(eliminar->clave);
	free(eliminar);
	hash->capacidad--;
	return removido;
}

/*
 * Recorre las colisiones de una posicion de la tabla y libera 
 * la memoria de cada nodo.
 */
void liberar_nodos(struct nodo *actual, void (*destructor)(void *))
{
	if (actual == NULL)
		return;
	if (destructor)
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
size_t recorrer_nodos(struct nodo *actual,
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

/*
 * Recibe un hash y un array que contiene los nodos que se quieren insertar
 * en el hash.
 */
void cargar_nodos(struct hash *hash, struct nodo **array)
{
	for (size_t i = 0; i < hash->tamanio >> 1; i++) {
		struct nodo *actual = array[i];
		while (actual != NULL) {
			size_t posicion =
				djb2((void *)actual->clave) % hash->tamanio;
			void *aux = actual->siguiente;
			insertar_nodo(hash, posicion, actual);
			actual = aux;
		}
	}
}

/*
 * Recibe el hash a rehashear. 
 *
 * Devuelve el hash rehasheado o NULL en caso de error.
 */
void *rehash(struct hash *hash)
{
	struct nodo **array = hash->array;
	size_t nuevo_tamanio = hash->tamanio << 1;

	hash->array = calloc(1, (nuevo_tamanio) * sizeof(struct nodo *));
	if (!hash->array) {
		hash->array = array;
		return NULL;
	}
	hash->tamanio = nuevo_tamanio;
	hash->capacidad = 0;

	cargar_nodos(hash, array);

	free(array);
	return hash;
}

hash_t *hash_crear(size_t capacidad)
{
	struct hash *hash = calloc(1, sizeof(struct hash));
	if (!hash)
		return NULL;

	hash->tamanio = capacidad < CANT_MINIMA ? CANT_MINIMA : capacidad;
	hash->array = calloc(1, hash->tamanio * sizeof(struct nodo *));
	if (!hash->array) {
		free(hash);
		return NULL;
	}
	return hash;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = djb2((void *)clave) % hash->tamanio;
	void *remplazado = NULL;

	struct nodo *existe = buscar_clave(hash->array[posicion], clave);
	if (!existe) {
		if (!insertar_nodo(hash, posicion, crear_nodo(clave, elemento)))
			return NULL;
	} else {
		remplazado = existe->valor;
		existe->valor = elemento;
	}
	if (anterior)
		*anterior = remplazado;

	float factor_carga = (float)hash->capacidad / (float)hash->tamanio;

	return factor_carga >= FACTOR_CARGA_MAXIMO ? rehash(hash) : hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = djb2((void *)clave) % hash->tamanio;
	if (!hash->array[posicion])
		return NULL;

	return quitar_nodo(hash, posicion, clave);
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = djb2((void *)clave) % hash->tamanio;
	struct nodo *buscado = buscar_clave(hash->array[posicion], clave);

	return !buscado ? NULL : buscado->valor;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = djb2((void *)clave) % hash->tamanio;
	return !!buscar_clave(hash->array[posicion], clave);
}

size_t hash_cantidad(hash_t *hash)
{
	return !hash ? 0 : hash->capacidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (size_t i = 0; i < hash->tamanio; i++)
		liberar_nodos(hash->array[i], destructor);
	free(hash->array);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	if (!hash || !f)
		return 0;

	size_t cantidad = 0;
	bool sigo = true;
	for (size_t i = 0; i < hash->tamanio && sigo; i++)
		cantidad += recorrer_nodos(hash->array[i], f, aux, &sigo);

	return cantidad;
}
