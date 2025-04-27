import sys
import os
import re
from collections import defaultdict

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <path to index logs>")
        exit(1)

    path = sys.argv[1]
    indexes = defaultdict(dict)
    for file in os.listdir(path):
        full_path = os.path.join(path, file)
        with open(full_path, 'r') as f:
            lines = f.readlines()
            for line in lines:
                match = re.search(r'Offset reached (\d+)', line)
                if match is None:
                    continue
                offset = int(match.group(1))
                indexes[file]["numtokens"] = indexes[file].get("numtokens", 0) + offset
            match = re.search(r'number of index chunks: (\d+)', lines[-2])
            indexes[file]["numchunks"] = int(match.group(1))
            match = re.search(r'Number of documents indexed so far (\d+)', lines[-1])
            indexes[file]["numdocuments"] = int(match.group(1))

    totaltokens = 0
    totalchunks = 0
    totaldocuments = 0
    for index, stats in indexes.items():
        print(index)
        print("\tNum tokens:", stats["numtokens"])
        print("\tNum chunks:", stats["numchunks"])
        print("\tNum documents:", stats["numdocuments"])
        totaltokens += stats["numtokens"]
        totalchunks += stats["numchunks"]
        totaldocuments += stats["numdocuments"]

    print("Total")
    print("\tNum tokens:", totaltokens)
    print("\tNum chunks:", totalchunks)
    print("\tNum documents:", totaldocuments)
