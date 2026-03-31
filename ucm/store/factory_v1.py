import importlib
from typing import Callable

from ucm.logger import init_logger
from ucm.store.ucmstore_v1 import UcmKVStoreBaseV1

logger = init_logger(__name__)


class UcmConnectorFactoryV1:
    _registry: dict[str, Callable[[], type[UcmKVStoreBaseV1]]] = {}

    @classmethod
    def register_connector(cls, name: str, module_path: str, class_name: str) -> None:
        """Register a connector with a lazy-loading module and class name."""
        if name in cls._registry:
            raise ValueError(f"Connector '{name}' is already registered.")

        def loader() -> type[UcmKVStoreBaseV1]:
            module = importlib.import_module(module_path)
            return getattr(module, class_name)

        cls._registry[name] = loader

    @classmethod
    def create_connector(
        cls, connector_name: str, config: dict, module_path: str = None
    ) -> UcmKVStoreBaseV1:
        if module_path:
            module = importlib.import_module(module_path)
            connector_cls = getattr(module, connector_name)
        elif connector_name in cls._registry:
            connector_cls = cls._registry[connector_name]()
        else:
            raise ValueError(f"Unsupported connector type: {connector_name}")
        assert issubclass(connector_cls, UcmKVStoreBaseV1)
        logger.info(f"Creating connector with name: {connector_name}")
        return connector_cls(config)


UcmConnectorFactoryV1.register_connector(
    "UcmNfsStore", "ucm.store.pcstore.pcstore_connector_v1", "UcmPcStoreV1"
)
UcmConnectorFactoryV1.register_connector(
    "UcmPipelineStore", "ucm.store.pipeline.connector", "UcmPipelineStore"
)
