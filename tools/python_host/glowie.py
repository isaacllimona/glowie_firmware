import asyncio

from bleak import BleakClient, BleakScanner

discover_srvc_uuid = "00000000-1fbd-0481-2a47-f3b657518ec1"
cmd_in_uuid = "00000001-1fbd-0481-2a47-f3b657518ec1"


def compact_script(script: str) -> str:
    """Make script more compact by removing extra whitespaces"""
    return " ".join(script.split())


class Glowie:
    def __init__(self, device):
        self.event_loop = asyncio.new_event_loop()
        self.ble_client = BleakClient(device)

    def __enter__(self):
        self.event_loop.run_until_complete(self.ble_client.connect())

    def __exit__(self, *args):
        self.event_loop.run_until_complete(self.ble_client.disconnect())

    def send_command(self, script: str):
        assert self.ble_client.is_connected, "Glowie is not connected"
        compacted = compact_script(script)
        data = compacted.encode()
        self.event_loop.run_until_complete(
            self.ble_client.write_gatt_char(cmd_in_uuid, data, response=True)
        )


def filter(dev, adv) -> bool:
    return discover_srvc_uuid in adv.service_uuids


def find_glowie() -> Glowie | None:
    device = asyncio.run(BleakScanner.find_device_by_filter(filter))
    if device is None:
        return None
    else:
        return Glowie(device)
