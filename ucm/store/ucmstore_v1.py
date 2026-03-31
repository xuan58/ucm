from abc import ABC, abstractmethod
from typing import Dict, List

import numpy as np
import torch


class Task(ABC):
    """Asynchronous task handle returned by transfer operations.

    This is an opaque token that can be polled or awaited.
    """

    pass


class UcmKVStoreBaseV1(ABC):
    """Abstract base class for KV-cache-centric storage backends.

    A concrete storage vendor must implement this interface to participate in
    the unified-cache-management (UCM) system.
    """

    def __init__(self, config: Dict[str, object]) -> None:
        """Initialize the store with vendor-specific configuration.

        Args:
            config: Key-value mapping containing vendor-specific parameters
                (e.g., connection string, cache size, compression level).
        """
        self.config = config

    @abstractmethod
    def cc_store(self) -> int:
        """Return a low-level C/C++ pointer to the underlying store.

        Returns:
            An opaque ``int`` representing the ``Store*`` instance that can
            be passed to native code.
        """
        pass

    @abstractmethod
    def lookup(self, block_ids: List[bytes]) -> List[bool]:
        """Check presence of blocks in external storage.

        Args:
            block_ids: List of vLLM block hashes (raw bytes).

        Returns:
            A list of booleans, ``True`` if the corresponding block exists in
            storage, ``False`` otherwise. The order matches ``block_ids``.
        """
        pass

    @abstractmethod
    def lookup_on_prefix(self, block_ids: List[bytes]) -> int:
        """Check presence of blocks in external storage.

        Args:
            block_ids: List of vLLM block hashes (raw bytes).

        Returns:
            An index representing the maximum index of blocks found in storage,
            returns -1 if none are found.
        """
        pass

    @abstractmethod
    def prefetch(self, block_ids: List[bytes]) -> None:
        """Asynchronously prefetch blocks into high-speed cache.

        Args:
            block_ids: List of vLLM block hashes to prefetch.
        """
        pass

    @abstractmethod
    def load(
        self,
        block_ids: List[bytes],
        shard_index: List[int],
        dst_tensor: List[List[torch.Tensor]],
    ) -> Task:
        """Initiate transfer of KV cache from storage to device.

        Args:
            block_ids: Hashes of the blocks to load.
            shard_index: Shard index for each block.
            dst_tensor: Double-list structure where ``dst_tensor[i][j]`` is the
                destination PyTorch tensor on device for block ``i``, tensor ``j``.

        Returns:
            A ``Task`` handle that can be used to check or wait for completion.
        """
        pass

    @abstractmethod
    def dump(
        self,
        block_ids: List[bytes],
        shard_index: List[int],
        src_tensor: List[List[torch.Tensor]],
    ) -> Task:
        """Initiate transfer of KV cache from device to storage.

        Args:
            block_ids: Hashes of the blocks to write.
            shard_index: Shard index for each block.
            src_tensor: Double-list structure where ``src_tensor[i][j]`` is the
                source PyTorch tensor on device for block ``i``, tensor ``j``.

        Returns:
            A ``Task`` handle that can be used to check or wait for completion.
        """
        pass

    @abstractmethod
    def load_data(
        self,
        block_ids: List[bytes],
        shard_index: List[int],
        dst_addr: List[List[int]] | np.ndarray,
    ) -> Task:
        """Low-level fetch: copy KV data to device pointers.

        Args:
            block_ids: Block hashes to load.
            shard_index: Shard index for each block.
            dst_addr: Double-list of ``int`` pointers (as Python ``int``) to
                pre-allocated device buffers.

        Returns:
            A ``Task`` handle for the asynchronous copy.
        """
        pass

    @abstractmethod
    def dump_data(
        self,
        block_ids: List[bytes],
        shard_index: List[int],
        src_addr: List[List[int]] | np.ndarray,
        prerequisite_handle: int = 0,
    ) -> Task:
        """Low-level dump: copy KV data from device pointers.

        Args:
            block_ids: Block hashes to store.
            shard_index: Shard index for each block.
            src_addr: Double-list of ``int`` pointers to device buffers.
            prerequisite_handle: Optional event handle for Python-C++ stream sync.
                When non-zero, cache stream waits for this event before D2H.

        Returns:
            A ``Task`` handle for the asynchronous copy.
        """
        pass

    @abstractmethod
    def wait(self, task: Task) -> None:
        """Block until the given transfer task completes.

        Args:
            task: Task handle returned by ``load``, ``dump``, ``load_data``,
                or ``dump_data``.
        """
        pass

    @abstractmethod
    def check(self, task: Task) -> bool:
        """Non-blocking poll for task completion.

        Args:
            task: Task handle returned by any transfer method.

        Returns:
            ``True`` if the task has finished, ``False`` if still in-flight.
        """
        pass
