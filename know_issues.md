# Known issues / TODO List

## General
 - No common error handling mechanisms. Sometimes the library just makes the program
 to end abruptly, some others the error is handled at application level, and
 some others the error is not handled. Define clear error codes per module so
 that they indicate the produced error as well as the mechanisms to handle them.
 - The examples does not take care of the ownership of the memory allocated by
 the API.
 - Destroy methods does not release the resources allocated during creation. As
 an example, timers does not stop callbacks when destroyed so callbacks will be
 fired internally.
 - Destroy method should not call release reference count... Destroy should be
 called as the action to release all of the resources when reference count come
 to zero. Better to define a release method invoking the destroy when no more
 references are pointing to the 'object'.
 - All mandatory parameters defined at the API (public functions),
 should be checked with assertion to facilitate debugging. In the case that
 other parameters requires to be checked at runtime, they should be checked
 at the beginning of the method and propagate the error when proceeds.
- Some private structures are used in public API
(e.g. sbfBatch sbfBatch_create (struct sbfTportImpl* tport)).
