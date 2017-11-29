/*!
   \file sbfKeyValue.h
   \brief This file declares the structures and functions to manage
   an in memory key value table.
   \copyright © Copyright 2016 Neueda all rights reserved.
 */

#ifndef _SBF_KEY_VALUE_H_
#define _SBF_KEY_VALUE_H_

#include "sbfCommon.h"

SBF_BEGIN_DECLS

/** Declares the key value handler. */
typedef struct sbfKeyValueImpl* sbfKeyValue;

/**
 * Creates a key value table and returns its handler.
 * @return the key value table's handler.
 */
sbfKeyValue sbfKeyValue_create (void);

/**
 * Releases the resources allocated by the create operation.
 * @param table the key value table's handler.
 */
void sbfKeyValue_destroy (sbfKeyValue table);

/**
 * Returns a copy of the key value table.
 * @param table the table to be copied.
 * @return a copy of the key value table.
 */
sbfKeyValue sbfKeyValue_copy (sbfKeyValue table);

/**
 * Returns the size of the key value table.
 * @param table the key value table's handler.
 * @return the size of the key value table.
 */
u_int sbfKeyValue_size (sbfKeyValue table);

/**
 * Returns the value from a given key. Returns a null terminating string
 * containing the value or null if the key was not found.
 * @param table the key value table's handler.
 * @param key null terminating string containing the key.
 * @return the value from a given key. Returns a null terminating string
 * containing the value or null if the key was not found.
 */
const char* sbfKeyValue_get (sbfKeyValue table, const char* key);

/**
 * Returns the value from a given key in an sprintf format like.
 * Returns a null terminating string containing the value
 * or null if the key was not found.
 * @param table the key value table's handler.
 * @param fmt The string containing the format of the key
 * @param ... the list of parameters that will compose the key.
 * @return the value from a given key. Returns a null terminating string
 * containing the value or null if the key was not found.
 */
const char* sbfKeyValue_getV (sbfKeyValue table,
                              const char* fmt, ...) SBF_PRINTFLIKE(2, 3);

/**
 * Adds an key and value to the key value table.
 * @param table the key value table's handler.
 * @param key null terminating string containing the key.
 * @param value the value to be associated to the given key.
 */
void sbfKeyValue_put (sbfKeyValue table, const char* key, const char* value);

/**
 * Removes a key value from the given key value table.
 * Note that the calling on this method will invalidate first/next.
 * @param table the key value table's handler.
 * @param key identifying the item to be removed.
 */
void sbfKeyValue_remove (sbfKeyValue table, const char* key);

/**
 * This method will return the first element, and can be used together with
 * sbfKeyValue_next to iterate over the elements stored in the key
 * value table.
 * @param table the key value table's handler.
 * @param cookie the key value item placed at the first position.
 * @return the value returned at the first position.
 * \see sbfKeyValue_next
 */
const char* sbfKeyValue_first (sbfKeyValue table, void** cookie);

/**
 * This method will return the next element stored in the key
 * value table.
 * @param table the key value table's handler.
 * @param cookie the key value item placed at the next position.
 * @return the value returned at the next position.
 * \see sbfKeyValue_first
 */
const char* sbfKeyValue_next (sbfKeyValue table, void** cookie);

SBF_END_DECLS

#endif /* _SBF_KEY_VALUE_H_ */
