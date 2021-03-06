from micropython import const
from ubinascii import hexlify

from trezor import ui
from trezor.messages import ButtonRequestType
from trezor.ui.container import Container
from trezor.ui.qr import Qr
from trezor.ui.text import Text
from trezor.utils import chunks

from apps.common import HARDENED
from apps.common.confirm import confirm, require_confirm


async def show_address(ctx, address: str, address_n: list, network: str = None):
    text = Text("Confirm address", ui.ICON_RECEIVE, icon_color=ui.GREEN)
    # TODO: print address_n via address_n_to_str(address_n)
    if network:
        text.normal("%s network" % network)
    text.mono(*split_address(address))
    return await confirm(
        ctx, text, code=ButtonRequestType.Address, cancel="QR", cancel_style=ui.BTN_KEY
    )


async def show_qr(ctx, address: str):
    qr_x = const(120)
    qr_y = const(115)
    qr_coef = const(4)

    qr = Qr(address, (qr_x, qr_y), qr_coef)
    text = Text("Confirm address", ui.ICON_RECEIVE, icon_color=ui.GREEN)
    content = Container(qr, text)
    return await confirm(
        ctx,
        content,
        code=ButtonRequestType.Address,
        cancel="Address",
        cancel_style=ui.BTN_KEY,
    )


async def show_pubkey(ctx, pubkey: bytes):
    lines = chunks(hexlify(pubkey).decode(), 18)
    text = Text("Confirm public key", ui.ICON_RECEIVE, icon_color=ui.GREEN)
    text.mono(*lines)
    return await require_confirm(ctx, text, code=ButtonRequestType.PublicKey)


def split_address(address: str):
    return chunks(address, 17)


def address_n_to_str(address_n: list) -> str:
    def path_item(i: int):
        if i & HARDENED:
            return str(i ^ HARDENED) + "'"
        else:
            return str(i)

    return "m/" + "/".join([path_item(i) for i in address_n])
