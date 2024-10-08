
#include "misc/Js.h"
#include "misc/Log.h"
#include "misc/Strings.h"
#include "misc/kafkawrap/KafkaReaderMock.h"
#include "misc/kafkawrap/KafkaWriter.h"
#include "scheduler/default/Scheduler.h"
#include "tests/common/test_datasets.h"

using namespace contentv1;

// load gened visits dataset to kafka

// run in workspace dir
// std::string config_path = "./src/bin/sched/cli/ut/sched_ut.conf";
std::string config_path = "./src/bin/contentd/cli/ut/contentd_ut.conf";
std::string dataset_path = "." + s_gened_visits_dataset_rel_path;

int main(int argc, char **argv) {

  LOG("kafka_loader: parsing scheduler config: " << config_path);

  SchedulerConfig cfg;
  cfg.parseFile(config_path);
  cfg.validate("main:");

  std::string push_topic =
      cfg[SchedulerConfig::STR_KAFKA_INPUT_STREAM_VISITS_TOPIC]
          .as<std::string>();
  std::vector<std::string> brokers =
      misc::Config::getStrArray(cfg[SchedulerConfig::STR_ARRAY_KAFKA_BROKERS]);

  LOG("Loading dataset: " << dataset_path);
  auto gened_visits_dataset =
      misc::KafkaReaderMock::loadNewlineFile(dataset_path);
  LOG("DONE, visits: " << gened_visits_dataset.size());

  // write to kafka
  {
    LOG("Connecting to brokers: " << misc::vec_to_csv_str(brokers));
    LOG("Writing to topic: " << push_topic);
    misc::KafkaWriterI::Ptr kafka_writer{new misc::KafkaWriter{brokers}};
    for (size_t i = 0; i < gened_visits_dataset.size(); ++i) {
      kafka_writer->write(push_topic, gened_visits_dataset[i]);
    }
    kafka_writer->flush(2000);
  }
  LOG("DONE, exiting..");
  return 0;
}
