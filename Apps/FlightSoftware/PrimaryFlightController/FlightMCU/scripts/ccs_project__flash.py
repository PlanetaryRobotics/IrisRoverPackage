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
    logger.info(
        "Connected to TI CCS with: \n"
        f"{'\n'.join([f'{k}: {v}' for k,v in tiflash.get_info().items()])}"
    )
