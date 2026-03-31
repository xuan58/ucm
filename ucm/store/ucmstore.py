from abc import ABC, abstractmethod
from typing import Dict, List, Tuple

import torch


class Task:
    """
    Abstract Task for kv transfer
    """

    pass


class UcmKVStoreBase(ABC):
    """
    Storage vendor implements this interface to support KV Cache centric inference system.
    """

    def __init__(self, config: Dict):
        self.config = config

    @abstractmethod
    def cc_store(self) -> int:
        """
        get the underlying implementation of Store

        Returns:
            cc pointer to Store
        """
        pass

    @abstractmethod
    def create(self, block_ids: List[str]) -> List[int]:
        """
        create kv cache space in storafe

        Args:
            block_ids (List[str]): vLLM block hash.

        Returns:
            0 - success
            others - failed
        """
        pass

    @abstractmethod
    def lookup(self, block_ids: List[str]) -> List[bool]:
        """
        Get number of blocks that can be loaded from the
        external KV cache.

        Args:
            block_ids (List[str]): vLLM block hash.

        Returns:
            hit block mask, True -> hit
        """
        pass

    @abstractmethod
    def prefetch(self, block_ids: List[str]) -> None:
        """
        prefetch kv cache to high speed cache according to block_ids.

        Args:
            block_ids (List[str]): vLLM block hash.
        """
        pass

    @abstractmethod
    def load(
        self, block_ids: List[str], offset: List[int], dst_tensor: List[torch.Tensor]
    ) -> Task:
        """
        load kv cache to device.

        Args:
            block_ids (List[str]): vLLM block hash.
            offset(List[int]): tp > 1 scene
            dst_tensor: List[torch.Tensor]: device tensor addr.
        Returns:
            task(Task).
        """
        pass

    @abstractmethod
    def dump(
        self, block_ids: List[str], offset: List[int], src_tensor: List[torch.Tensor]
    ) -> Task:
        """
        dump kv cache from device.

        Args:
            block_ids (List[str]): vLLM block hash.
            offset(List[int]): tp > 1 scene
            src_tensor: List[torch.Tensor]: device tensor addr.
        Returns:
            task(Task).
        """
        pass

    @abstractmethod
    def fetch_data(
        self,
        block_ids: List[str],
        offset: List[int],
        dst_addr: List[int],
        size: List[int],
    ) -> Task:
        """
        load kv cache data to device.

        Args:
            block_ids (List[str]): vLLM block hash.
            offset(List[int]): tp > 1 scene
            dst_addr: List[int]: device tensor addr ptr.
            size: List[int]: device tensor size.
        Returns:
            task(Task).
        """
        pass

    @abstractmethod
    def dump_data(
        self,
        block_ids: List[str],
        offset: List[int],
        src_addr: List[int],
        size: List[int],
    ) -> Task:
        """
        dump kv cache data from device.

        Args:
            block_ids (List[str]): vLLM block hash.
            offset(List[int]): tp > 1 scene
            src_addr: List[int]: device tensor addr ptr.
            size: List[int]: device tensor size.
        Returns:
            task(Task).
        """
        pass

    @abstractmethod
    def wait(self, task: Task) -> int:
        """
        wait kv cache kv transfer task finished.

        Args:
            task (Task): transfer engine task.
        Returns:
            0 - success
            others - failed.
        """
        pass

    @abstractmethod
    def commit(self, block_ids: List[str], is_success: bool = True) -> None:
        """
        commit kv cache, now kv cache can be reused.

        Args:
            block_ids (List[str]): vLLM block hash.
            is_success(bool): if False, we need release block
        """
        pass

    @abstractmethod
    def check(self, task: Task) -> Tuple[int, bool]:
        """
        check if kv transfer task finished.

        Args:
            task (Task): transfer engine task.
        Returns:
            ret: 0 means success, others means failed
            finished: True means finished, False means in process
        """
        pass
