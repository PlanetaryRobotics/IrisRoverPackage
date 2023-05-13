"""
Builds the Dash backend used for queueing jobs and sharing data.

Author: Connor W. Colombo (colombo@cmu.edu)
Last Updated: 05/09/2023
"""
from typing import Any, TypeGuard
import os
import io

import json
import plotly
import pandas as pd
from enum import Enum
import redis
from celery import Celery
from dash import CeleryManager

from .context import GuiContext


def _get_redis_url() -> str:
    # Use default broker address if not in environ:
    return os.environ.get('REDIS_URL', 'redis://localhost:6379/0')


class DatabaseKey(Enum):
    """Registry of all valid Keys to the Database
    (having a registry prevents typos and allows for key changes later)."""
    TELEM_DF = 'telem_df'
    PACKET_DF = 'packet_df'


class InternalDatabase:
    """
    Internal database full of data loaded from IPC and pre-processed.
    Data is written from the IPC layer whenever new data arrives and is read 
    from the Dash GUI layer at a fixed update interval.

    Currently this uses Redis for in-RAM storage but this wrapper should make
    the GUI implementation agnostic.
    """
    context: GuiContext
    _redis: redis.StrictRedis

    def __init__(
        self,
        context: GuiContext,
        db_url: str | None = None
    ) -> None:
        if db_url is None:
            db_url = _get_redis_url()
        self._redis = redis.StrictRedis.from_url(db_url)
        # Keep a handle for the GuiContext around:
        self.context = context

    def save(self, key: DatabaseKey, value: Any) -> None:
        if isinstance(value, pd.DataFrame):
            buffer = io.BytesIO()
            value.to_parquet(buffer, compression='gzip')
            buffer.seek(0)
            df_as_bytes = buffer.read()
            serialized_type = 'pd.DataFrame'
            serialized_value = df_as_bytes
        else:
            serialized_value = json.dumps(
                value,
                cls=plotly.utils.PlotlyJSONEncoder
            ).encode('utf-8')
            serialized_type = 'json-serialized'

        self._redis.set(f'{key.value}_value', serialized_value)
        self._redis.set(f'{key.value}_type', serialized_type)

    def read(self, key: DatabaseKey) -> Any:
        serialized_value = self._redis.get(f'{key.value}_value')
        serialized_type = self._redis.get(f'{key.value}_type')
        try:
            if serialized_type == b'pd.DataFrame':
                value = pd.read_parquet(
                    io.BytesIO(serialized_value))  # type: ignore
            else:
                value = json.loads(serialized_value)  # type: ignore
        except Exception as e:
            self.context.ipc_app.logger.error(
                f"[Internal Database] Failed to load key=`{key}`. "
                f"Serialized type=`{serialized_type}`, "
                f"value=`{serialized_value}`. "
                f"Because: `{e}`."
            )
        return value


def db_ready(db: Any | None) -> TypeGuard[InternalDatabase]:
    """Type narrowing type guard that implicitly determines whether the DB is
    ready (initialized and present)."""
    return db is not None and isinstance(db, InternalDatabase)


def make_callback_manager(context: GuiContext) -> CeleryManager:
    redis_url = _get_redis_url()
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
