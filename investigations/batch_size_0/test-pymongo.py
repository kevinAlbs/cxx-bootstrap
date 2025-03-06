from pymongo import MongoClient
from pymongo import monitoring
from bson import json_util
import json

# Command listener to handle the command started events
class CommandLogger(monitoring.CommandListener):
    def started(self, event):
        print(f"Command {event.command_name} started {event.command}")

    def succeeded(self, _):
        pass

    def failed(self, _):
        pass

# Register the command listener
monitoring.register(CommandLogger())

def watch_change_stream():
    client = MongoClient("mongodb://localhost:27017")

    collection = client['db']['coll']

    # Create a change stream on the collection
    try:
        with collection.watch(batch_size=0) as stream: # sends batchSize:0 for aggregate, and no batchSize for getMore.
            print("Waiting for change stream events...")
            for change in stream:
                print("Change detected:", json.dumps(change, indent=4, default=json_util.default))

    except Exception as e:
        print("An error occurred while watching the change stream:", e)

if __name__ == "__main__":
    watch_change_stream()
