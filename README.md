<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TDA HASH

## Repositorio de Francisco Infanti - 110822 - finfanti@fi.uba.ar

- Para compilar:

```bash
make pruebas_chanutron 
```

- Para ejecutar:

```bash
./pruebas_chanutron 
```

- Para ejecutar con valgrind:
```bash
make valgrind-chanutron
```
---
##  Funcionamiento
El programa consiste en la implementacion de un diccionario mediante una `tabla de hash` de tipo `abierta` con direccionamiento `cerrado`.

La implementacion que se utilizo para crear la tabla de hash consiste de dos estructuras: 
- Una principal, `struct hash`, la cual almacenara el tamaño de la tabla, la cantidad de espacio que estoy utilizando y un vector de punteros. Cada posicion de dicho vector apuntara a la segunda estructura. 
- La segunda estructura, `struct nodo`,  almacenara la direccion de memoria del valor que se esta guardando, una copia de la clave y la direccion de memoria del siguiente nodo.

---
<div align="center">
<img width="40%" src="img/estructura_hash.svg">
</div>

---
En esta implementacion, al ser un `hash abierto`, debemos posicionar a las colisiones en las posiciones que les corresponde. Por lo tanto para resolver este problema lo que hago es hacer uso del puntero al siguiente que tengo en cada nodo, para ir encadenando las colisiones a medida que ocurren. En el caso de que no haya colisiones dicho nodo apuntaria a `NULL`.

La funcion de hash que decidi utilizar es ***djb2***, esta recibe un string y lo transorma en una serie de numeros aleatorios. Para que dichos numero aleatorios se mantengan dentro del rango de posiciones de mi vector, aplico el operador modulo. Es decir, si $n$ es numero que me devuelve ***djb2***, $n$ % $m$, siendo $m$ el tamaño de la tabla, me devuelve la posicion de la tabla donde debo insertarlo.

El usuario puede guardar cualquier tipo de valor en el diccionario, por eso me aprovecho de los `void *` que me permiten guardar direcciones de memoria sin necesidad de saber que hay en dichas, pero la clave que va a ser utilizada debe ser un string.

Al usuario se le proporcionan una variedad de operaciones las cuales puede utilizar para poder interactuar con el diccionario, por ejemplo: *crear*, *destruir*, *insertar*, *quitar*, *buscar*, etc...

Para poder ***crear*** un hash lo que se hace es reservar un bloque de memoria que pueda almacenar todo lo especificado en `struct hash`. A su vez, dentro de la estructura debemos reservar un array de punteros a `struct nodo` de un tamaño especificado por el usuario, este tamaño no puede ser menor a tres. Al finalizar el proceso de reservar memoria e inicializar los campos en sus valores correspondientes se le devuelve al usuario un puntero a la estructura principal.

*(***COMENTARIO***: Inicialmente todos los valores de los campos de la estrctura se inicializaran en $0$, menos el campo que almacena el tamaño del array, en ese caso se inicializara con el tamaño que especifico el usuario.)*

Si analizamos la complejidad de crear un hash, podemos ver que esta es constante $O(1)$, pues solamente estamos reservando bloques de memoria en el heap e inicializandolos en el valor correspondiente.

---
<div align="center">
<img width="35%" src="img/hash_crear.svg">
</div>

---
Una vez hayamos creado el hash debemos destruirlo una vez terminemos de usarlo. Para realizar dicha operacion se proporcionan dos funciones, ambas van a liberar la memoria que hash, pero con la diferencia de que una aplicara una funcion destructora a cada elemento. La forma mas sencilla de destruir el hash, es ir recorriendo el array y liberar la memoria de cada nodo. Una vez se termina de recorrer, liberamos el array y por ultimo la estructura principal. 

Veamos que la complejidad de destruir un hash siempre va a ser $O(n)$, pues si nosotros almacenamos $n$ elementos, debemos liberar la memoria de cada uno de ellos, por lo tanto debemos recorrer los $n$ elementos.

---
<div align="center">
<img width="40%" src="img/hash_destruir.svg">
</div>

---


## Respuestas a las preguntas teóricas
Un diccionario es una coleccion de pares, un par esta conformado por dos partes: 
- El `valor`, que vendria a ser lo que se quiere almacenar en el diccionario.
- La `clave`, esta debe ser unica y es utilizada como un indice, pues a traves de esta vamos a poder encontrar y acceder al `valor` que se esta almacenando. 

El motivo de por el cual usamos diccionarios es porque estos mejoran el tiempo de acceso a un elemento. Es decir, veamos en el caso de las **lista**, el acceso a un dato tiene una complejidad lineal $O(n)$. En el caso de los **ABB** (suponiendo que esta balanceado), la complejidad seria $O(log(n))$. Pero en el caso de los diccionarios, se busca optimizar esta operacion a tal punto de que la complejidad sea $O(1)$, entonces buscar un elemento en un dicionaros con $n$ elementos seria algo casi instantaneo. 

### Tablas de Hash

Una posible forma de implementar los diccionarios es con ***Tablas de Hash***. Esta es una estructura la cual contiene estos pares mencionados anteriormente. 

Yo para poder acceder a un elemento de la tabla voy a necesitar la `clave` que esta asociada a este elemento. Teniendo mi `clave` voy a tener que aplicarle una `funcion de hash`, lo que hace esta funcion es transformar esa `clave` en una posicion de la tabla, donde puede que se encuentre el elemento que estoy buscando. Si el elemento existe, entonces debe estar en esa posicion.

---
<div align="center">
<img width="70%" src="tabla_de_hash.svg">
</div>

---
El problema que ocurre es que yo puedo tener $n$ claves y $m$ posiciones en tabla.Ahora $n$ es un valor que no esta acotado, puede tomar valores desde $0$ hasta el infinito. Pero $m$ si es un valor acotado, este depende de la cantidad de espacio que tiene nuestra tabla de hash. Entonces lo que va a ocurrir, sea cual sea la funcion de hash, es que para $x$ valores de $n$ van a coincidir las posiciones en la tabla de hash. A esto se lo llaman ***Colisiones*** y dependiendo de como las resolvamos vamos a estar utilizando un tipo de hash diferente.

---
<div align="center">
<img width="70%" src="colisiones.svg">
</div>

---
- ### Hash Cerrado
  Se dice que cuando un hash es cerrado, este tiene un `direccionamiento abierto`.

  Que un hash sea cerrado significa que los pares deben guardarse dentro de la misma estructura que la tabla. Entonces podriamos pensar que cada posicion de la tabla va a almacenar un `valor` y una `clave`.

  ---
  <div align="center">
  <img width="70%" src="hash_cerrado.svg">
  </div>

  ---
  En este tipo de hash para poder manejar las `colisiones` existen tres formas diferentes:
  - `Porbing Lineal:` en este caso lo que haremos es insertar la colision en la proxima posicion vacia de la tabla. Es decir, nosotros vamos a hashear la clave y veremos que en la posicion obtenida ya hay un par. Entonces iremos avanzando por las posiciones de la tabla hasta encontrar una posicion libre e insertar el nuevo par.
  - `Porbing Cuadratico:` usaremos la operacion (intentos fallidos)$²$ para poder insertar. 
        
    - Por ejemplo si nuestra funcion de hash es $hash(clave)$ % $tamanio$, si encontramos una colision la primera vez haremos $hash(clave) +1²$ % $tamanio$, la segunda $hash(clave) +2²$ % $tamanio$ y la n-enesima $hash(clave) +n²$ % $tamanio$ 

  - `Hash Doble:` como lo dice el nombre aplicaremos otro hash diferente hasta encontrar un lugar libre en la tabla.

  Se dice que esta hash tiene un `direccionamiento abierto`, porque cuando insertamos un elemento lo estamos insertando en una posicion incorrecta.

  Para el caso del `Probing Lineal`, para pode buscar un elemento deberemos hashear la clave y a partir de esa posicion ir recorriendo la tabla hasta encontrar el elemento, una posicion vacia o volver al mismo lugar.

- ### Hash Abierto
  Se dice que cuando un hash es abierto, este tiene un `direccionamiento cerrado`.

  Se dice que es un hash abierto, porque almacenaremos los pares fuera de la estructura de la tabla, es decir, en cada posicion de la tabla habra un puntero a una estructura donde se encontraran los pares, esta estructura pueden ser **nodos**, **listas**, etc...

  Para trabajar con las colisiones lo que haremos es encadenar las colisiones en la estructura. Veamos que no hay problemas, pues si usamos una lista, podemos tener $n$ elementos en esa lista. Entonces nosotros hasheamos la clave, obtenemos la posicion y accedemos a la estructura donde debemos buscar el elemento recorriendola.

  Se dice que es un `direccionamiento cerrado`, porque las colisiones las estamos mandando a la posicion que les corresponde, solamente que las insertamos en una estructura a parte.

Ahora es logico pensar que al usar cualquier tipo de hash, la complejidad de busqueda no seria $O(1)$, pues en el caso del abierto (si usamos `probing lineal`) debemos recorrer $n$ elementos de la tabla hasta encontrar el elemento, una posicion vacia o volver al inicio. Y en el caso del cerrado debemos recorrer la estructura que puede tambien tener $n$ elementos. Ahora la realidad es que esto va a depender de que tan buena sea la funcion de hash que estamos utilizando. Ademas para poder mejorar esto, lo que hacemos es llevar la cuenta de un `factor de carga` y que cuando este factor supera cierto valor auementamos el tamaño de nuestra tabla. A esta operacion se la llama ***Rehashear*** y lo que hacemos es crear una nueva estructura y rehashear cada clave para volver a insertar todos los elementos.