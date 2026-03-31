import importlib
from typing import Callable

from ucm.logger import init_logger
from ucm.store.ucmstore import UcmKVStoreBase

logger = init_logger(__name__)


class UcmConnectorFactory:
    _registry: dict[str, Callable[[], type[UcmKVStoreBase]]] = {}

    @classmethod
    def register_connector(cls, name: str, module_path: str, class_name: str) -> None:
        """Register a connector with a lazy-loading module and class name."""
        if name in cls._registry:
            raise ValueError(f"Connector '{name}' is already registered.")

        def loader() -> type[UcmKVStoreBase]:
            module = importlib.import_module(module_path)
            return getattr(module, class_name)

        cls._registry[name] = loader

    @classmethod
    def create_connector(cls, connector_name: str, config: dict) -> UcmKVStoreBase:
        if connector_name in cls._registry:
            connector_cls = cls._registry[connector_name]()
        else:
            raise ValueError(f"Unsupported connector type: {connector_name}")
        assert issubclass(connector_cls, UcmKVStoreBase)
        logger.info(f"Creating connector with name: {connector_name}")
        return connector_cls(config)


UcmConnectorFactory.register_connector(
    "UcmNfsStore", "ucm.store.nfsstore.nfsstore_connector", "UcmNfsStore"
)
UcmConnectorFactory.register_connector(
    "UcmPcStore", "ucm.store.pcstore.pcstore_connector", "UcmPcStore"
)
UcmConnectorFactory.register_connector(
    "UcmMooncakeStore",
    "ucm.store.mooncakestore.mooncake_connector",
    "UcmMooncakeStore",
)
