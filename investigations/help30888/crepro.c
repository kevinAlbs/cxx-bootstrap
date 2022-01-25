#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
   mongoc_client_t *client;
   mongoc_collection_t *coll;
   char *uri;
   bson_t *filter;
   bson_error_t error;
   int64_t res;

   mongoc_init();

   uri = getenv ("MONGODB_URI");
   if (!uri) {
      MONGOC_ERROR ("expected environment variable MONGODB_URI to be set");
      return EXIT_FAILURE;
   }
   client = mongoc_client_new(uri);
   coll = mongoc_client_get_collection (client, "testDB", "testCol");
   filter = bson_new();
   res = mongoc_collection_count_documents (coll, filter, NULL /* opts */, NULL /* read prefs */, NULL /* reply */, &error);
   if (res < 0) {
      MONGOC_ERROR ("mongoc_collection_count_documents error: %s", error.message);
      return EXIT_FAILURE;
   }

   bson_destroy (filter);
   mongoc_collection_destroy (coll);
   mongoc_client_destroy (client);

   mongoc_cleanup ();
}
