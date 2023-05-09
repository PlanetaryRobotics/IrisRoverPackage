"""
Builds the Dash backend used for queueing jobs and sharing data.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/08/2023
"""
import os

from celery import Celery
from dash import CeleryManager

from gui.context import GuiContext


def make_callback_manager(context: GuiContext) -> CeleryManager:
    # Use default broker address if not in environ:
    redis_url = os.environ.get('REDIS_URL', 'redis://localhost:6379/0')
    try:
        # Use Redis & Celery if REDIS_URL set as an env variable
        celery_app = Celery(
            context.app_name,
            broker=redis_url,
            backend=redis_url
        )
        return CeleryManager(celery_app)
    except Exception as e:
        raise Exception(
            f"Can't make `CeleryManager` because: `{e}`. "
            "Has Redis Stack Server been started "
            "(`redis-stack-server` or `make run-infra`)?"
        )
