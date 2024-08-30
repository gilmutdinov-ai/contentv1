source "$(dirname "$0")/common.sh"

docker run -i -v D:/SourceCodes/contentv1:/sources/contentv1 -v D:/SourceCodes/contentv1/data:/export/contentv1/data --network contentv1_network docker.io/library/contentv1-dev bash
