# -p 3000:3000
docker run --rm -d --name=grafana \
  --network host \
  --volume grafana-storage:/var/lib/grafana \
  -e "GF_SERVER_ROOT_URL=http://localhost/" \
  -e "GF_INSTALL_PLUGINS=grafana-clock-panel" \
  grafana/grafana-enterprise
