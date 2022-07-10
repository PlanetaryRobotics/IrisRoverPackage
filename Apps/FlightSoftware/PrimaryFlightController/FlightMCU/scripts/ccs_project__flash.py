import tiflash
import logging

# Setup logging:
console = logging.StreamHandler()
# set a format which is simpler for console use
formatter = logging.Formatter('%(asctime)-15s: %(levelname)-8s %(message)s')
console.setFormatter(formatter)
logging.getLogger().addHandler(console)
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

if __name__ == "__main__":
    info_msg = '\n\t'.join([f'{k}: {v}' for k,v in tiflash.get_info().items()])
    logger.info(f"Connected to TI CCS with:\n\t{info_msg}")
