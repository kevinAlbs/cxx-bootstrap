/* Q: Why is maxTimeMS not used on aggregate in a change stream?
   A: Because failure to establish a cursor does not return a cursor ID.

   Run with mongo changestream_aggregate_maxtimems.js
 */
var local = db.getSiblingDB ("local");
var oldestOplogEntry = local.oplog.rs.find().sort({ts: 1}).next();
var oldestTimestamp = oldestOplogEntry["ts"]
var reply = db.runCommand({
    aggregate: "foo",
    pipeline: [{$changeStream: {startAtOperationTime: oldestTimestamp}}],
    cursor: {},
    maxTimeMS: 1})
printjson (reply)

/* Observed results on 4.4.3:

{
        "operationTime" : Timestamp(1639669744, 1),
        "ok" : 0,
        "errmsg" : "Error in $cursor stage :: caused by :: operation exceeded time limit",
        "code" : 50,
        "codeName" : "MaxTimeMSExpired",
        "$clusterTime" : {
                "clusterTime" : Timestamp(1639669744, 1),
                "signature" : {
                        "hash" : BinData(0,"AAAAAAAAAAAAAAAAAAAAAAAAAAA="),
                        "keyId" : NumberLong(0)
                }
        }
}
*/