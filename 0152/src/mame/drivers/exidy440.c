// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    Exidy 440 hardware

    driver by Aaron Giles

    Currently implemented:
        * Crossbow
        * Cheyenne
        * Combat/Catch-22
        * Crackshot
        * Chiller
        * Top Secret
        * Clay Pigeon
        * Hit 'n Miss
        * Who Dunit
        * Showdown
        * Yukon


440 System

Name                 Year  CPU    board/rom numbers

Crossbow             1983  6809   77-0019, 77-0022
Cheyenne             1984  6809
Combat               1985  6809
Chiller              1986  6809
Clay Pigeon          1986  6809
Crackshot            1986  6809
Hit 'n Miss          1986  6809
Top Secret           1986  6809
Showdown             1988  6809
Who Dunit            1988  6809


****************************************************************************

    Memory map

****************************************************************************

    ========================================================================
    CPU #1
    ========================================================================
    0000-1FFF   R/W   xxxxxxxx    Image RAM (64 images x 128 bytes, each image is 16x16x4bpp)
                R/W   xxxx----       (left pixel)
                R/W   ----xxxx       (right pixel)
    2000-209F   R/W   xxxxxxxx    Sprite RAM (40 entries x 4 bytes)
                R/W   xxxxxxxx       (0: Y position, inverted)
                R/W   -------x       (1: X position, upper bit, inverted)
                R/W   xxxxxxxx       (2: X position, lower 8 bits, inverted)
                R/W   --xxxxxx       (3: image number, inverted)
    20A0-29FF   R/W   xxxxxxxx    Stack RAM
    2A00-2AFF   R/W   xxxxxxxx    Scanline I/O region
                R/W   xxxx----       (left pixel)
                R/W   ----xxxx       (right pixel)
    2B00        R     xxxxxxxx    Current vertical beam position
    2B01        R     xxxxxxxx    Latched horizontal collision/beam detect location
    2B01        W     --------    Clear VBLANK FIRQ
    2B02        R/W   xxxxxxxx    Scanline I/O selector
    2B03        R     xxxxxxxx    Input port
                R     x-------       (VBLANK FIRQ latched, active low)
                R     -x------       (collision/beam detect FIRQ latched, active low)
                R     --xxxx--       (4 position DIP switch, active low)
                R     ------x-       (trigger, active low)
                R     -------x       (mirror of VBLANK FIRQ latch, Whodunit only)
    2B03        W     xxxxxxxx    Control port
                W     xxxx----       (ROM bank select, 0-15)
                W     ----x---       (collision/beam detect FIRQ enable)
                W     -----x--       (select FIRQ source: 0=collision, 1=beam detect)
                W     ------x-       (select palette bank for I/O)
                W     -------x       (select palette bank for display)
    2C00-2DFF   R/W   xxxxxxxx    Palette RAM (2 banks x 512 bytes)
                R/W   x-------       (0: collision trigger bit)
                R/W   -xxxxx--       (0: red component)
                R/W   ------xx       (0: green component, upper 2 bits)
                R/W   xxx-----       (1: green component, lower 3 bits)
                R/W   ---xxxxx       (1: blue component)
    2E00-2E1F   R/W   xxxxxxxx    Sound command
    2E20-2E3F   R     ----xxxx    Coin counter, clearing IRQ
                R     ----x---       (acknowledgement of sound command read, active high)
                R     -----x--       (unknown use, active low - gives "coin mech jammed" message)
                R     ------x-       (coin 2, active low)
                R     -------x       (coin 1, active low)
    2E40-2E5F   W     -------x    Clear coin counter
    2E60-2E7F   R     xxxxxxxx    8 position DIP switch, active low
    2E80-2E9F   R     --xxxxxx    Player control bits
                R     -------x       (start button, active low)
    2EA0-2EBF   R     ----xxxx    Coin counter, non-clearing
                R     ----x---       (acknowledgement of sound command read, active high)
                R     -----x--       (unknown use, active low - gives "coin mech jammed" message)
                R     ------x-       (coin 2, active low)
                R     -------x       (coin 1, active low)
    2EC0-2EDF   R     xxxxxxxx    Copy protection check (Clay Pigeon)
    2EC0-2EDF   R/W   xxxxxxxx    Copy protection and I/O space (Top Secret)
    3000-3FFF   R/W   xxxxxxxx    RAM
    4000-7FFF   R     xxxxxxxx    Banked ROM
    6000-7FFF   R/W   xxxxxxxx    EEROM (when bank 15 is selected only)
    8000-FFFF   R     xxxxxxxx    Program ROM
    ========================================================================
    Interrupts:
        NMI not connected
        IRQ generated by coins, detect on audio PCB
        FIRQ generated by VBLANK, collision detect, and beam detect
    ========================================================================


    ========================================================================
    CPU #2
    ========================================================================
    0000-7FFF   R     xxxxxxxx    Banked ROM (never access because DMA switches constantly)
    8000-8016   R/W   xxxxxxxx    MC6844 DMA controller I/O
    8400-8407   R/W   xxxxxxxx    Volume control for each channel: left1, right1, left2, right2...
    8800        R     xxxxxxxx    Sound command
    9400-9403   W     xxxxxxxx    Bank select for each channel
                W     ----x---       (select bank 3, active high)
                W     -----x--       (select bank 2, active high)
                W     ------x-       (select bank 1, active high)
                W     -------x       (select bank 0, active high)
    9800        W     --------    Clear sound IRQ
    A000-BFFF   R/W   xxxxxxxx    RAM
    E000-FFFF   R     xxxxxxxx    Program ROM
    ========================================================================
    Interrupts:
        NMI not connected
        IRQ generated by VSYNC
        FIRQ generated by write to sound command register
    ========================================================================

***************************************************************************

    Differences between machines (assuming Crossbow is the base)

    Cheyenne (reports Hardware Error 5 if any of these are not true)
        * a read from $2B03 must return 0 in bit 0

        conclusion: LS244 at 18J on main board needs to generate negative logic

    Combat (reports Hardware Error 1 if any of these are not true)
        * a read from $2E80 must return 0's in bits 1,2,3,4,5
        * a read from $2B03 must return 0 in bit 0

        conclusion: LS244 at 18J on main board needs to generate negative logic
                        LS367 at 1A on audio board needs to generate negative logic

    Crackshot (reports Hardware Error W if any of these are not true)
        * a read from $2E20 must return 0 in bit 2
        * a read from $2E80 must return 0's in bits 1,2,3,4,5
        * a read from $2B03 must return 0 in bit 0

        conclusion: LS244 at 18J on main board needs to generate negative logic
                        LS367 at 1A on audio board needs to generate negative logic
                        LS128 at 2A on audio board needs negative logic into pin 5

    Chiller (reports Hardware Error R if any of these are not true)
        * a read from $2E20 must return 0 in bit 2
        * a read from $2E80 must return 0's in bits 1,2,3,4,5
        * a read from $2B03 must return 0 in bit 0

        conclusion: LS244 at 18J on main board needs to generate negative logic
                        LS367 at 1A on audio board needs to generate negative logic
                        LS128 at 2A on audio board needs negative logic into pin 5

    Top Secret (reports Hardware Error H if any of these are not true)
        * a read from $2EC0 must return $00
        * a read from $2E20 must return 0 in bit 2
        * a read from $2E80 must return 0's in bits 2,3,4,5
        * a read from $2B03 must return 0 in bit 0

        conclusion: LS244 at 18J on main board needs to generate negative logic
                                and must connect the inputs for bits 0 and 1
                        LS367 at 1A on audio board needs to generate negative logic
                        LS128 at 2A on audio board needs negative logic into pin 5
                        the I/O space at $2EC0 must be hooked up to something

    Clay Pigeon (reports Hardware Error N if any of these are not true)
        * a read from $2EC3 must return $76
        * a read from $2E20 must return 0 in bit 2
        * a read from $2E80 must return 0's in bits 1,2,3,4,5
        * a read from $2B03 must return 0 in bit 0

        conclusion: LS244 at 18J on main board needs to generate negative logic
                        LS367 at 1A on audio board needs to generate negative logic
                        LS128 at 2A on audio board needs negative logic into pin 5
                        something must be hooked up to pin 7 of the LS42 at 3A on
                                the audio board to put $76 on the data lines going
                                back to the logic board when triggered

    Hit'N Miss (reports Hardware Error H if any of these are not true)
        * a read from $2E20 must return 0 in bit 2
        * a read from $2E80 must return 0's in bits 2,3,4,5
        * a read from $2B03 must return the same value for bit 0 as for bit 1

        conclusion: LS244 at 18J on main board needs to generate negative logic
                                and must connect the inputs for bits 0 and 1
                        LS367 at 1A on audio board needs to generate negative logic
                        LS128 at 2A on audio board needs negative logic into pin 5

    Who Dunit (reports Hardware Error H if any of these are not true)
        * a read from $2E20 must return 0 in bit 2
        * a read from $2E80 must return 0's in bits 2,3,4,5
        * a read from $2B03 must return the same value for bit 0 as for bit 7

        conclusion: LS244 at 18J on main board needs to generate negative logic
                                and must connect the inputs for bits 0 and 7
                        LS367 at 1A on audio board needs to generate negative logic
                        LS128 at 2A on audio board needs negative logic into pin 5

    Showdown
        * a read from $2E20 must return 0 in bit 2
        * a read from $2E80 must return 0's in bits 1,2,3,4,5
        * a read from $2B03 must return 0 in bit 0
        * the PLD at 1E on the logic board must recognize the sequence $0055,$00ed
            and return the following data from reads in the first 24 bytes:
                0x15,0x40,0xc1,0x8d,0x4c,0x84,0x0e,0xce,
                0x52,0xd0,0x99,0x48,0x80,0x09,0xc9,0x45,
                0xc4,0x8e,0x5a,0x92,0x18,0xd8,0x51,0xc0
        * the PLD at 1E on the logic board must also recognize the sequence $0055,$1243
            and return the following data from reads in the first 24 bytes:
                0x11,0x51,0xc0,0x89,0x4d,0x85,0x0c,0xcc,
                0x46,0xd2,0x98,0x59,0x91,0x08,0xc8,0x41,
                0xc5,0x8c,0x4e,0x86,0x1a,0xda,0x50,0xd1

        conclusion: something must emulate this PLD; also, not sure where the
                                bytes to compare against came from, so it's possible
                                that the real machine isn't expecting the same values
                        LS244 at 18J on main board needs to generate negative logic
                        LS367 at 1A on audio board needs to generate negative logic
                        LS128 at 2A on audio board needs negative logic into pin 5

***************************************************************************/

#include "emu.h"
#include "cpu/m6809/m6809.h"
#include "machine/nvram.h"
#include "includes/exidy440.h"


/* constants */
#define MAIN_CPU_CLOCK      (EXIDY440_MASTER_CLOCK / 8)






/*************************************
 *
 *  Coin handling
 *
 *************************************/

INPUT_CHANGED_MEMBER(exidy440_state::coin_inserted)
{
	/* if we got a coin, set the IRQ on the main CPU */
	if (oldval)
		m_maincpu->set_input_line(0, ASSERT_LINE);
}



/*************************************
 *
 *  Primary input port special bits
 *
 *************************************/

CUSTOM_INPUT_MEMBER(exidy440_state::firq_beam_r)
{
	return m_firq_beam;
}


CUSTOM_INPUT_MEMBER(exidy440_state::firq_vblank_r)
{
	return m_firq_vblank;
}


CUSTOM_INPUT_MEMBER(exidy440_state::hitnmiss_button1_r)
{
	/* button 1 shows up in two bits */
	UINT32 button1 = ioport("HITNMISS_BUTTON1")->read();
	return (button1 << 1) | button1;
}



/*************************************
 *
 *  Banked RAM (actually EEROM)
 *
 *************************************/

void exidy440_state::exidy440_bank_select(UINT8 bank)
{
	/* for the showdown case, bank 0 is a PLD */
	if (m_showdown_bank_data[0] != NULL)
	{
		if (bank == 0 && m_bank != 0)
			m_maincpu->space(AS_PROGRAM).install_read_handler(0x4000, 0x7fff, read8_delegate(FUNC(exidy440_state::showdown_bank0_r),this));
		else if (bank != 0 && m_bank == 0)
			m_maincpu->space(AS_PROGRAM).install_read_bank(0x4000, 0x7fff, "bank1");
	}

	/* select the bank and update the bank pointer */
	m_bank = bank;
	membank("bank1")->set_base(&memregion("maincpu")->base()[0x10000 + m_bank * 0x4000]);
}


WRITE8_MEMBER(exidy440_state::bankram_w)
{
	/* EEROM lives in the upper 8k of bank 15 */
	if (m_bank == 15 && offset >= 0x2000)
	{
		memregion("maincpu")->base()[0x10000 + 15 * 0x4000 + offset] = data;
		logerror("W EEROM[%04X] = %02X\n", offset - 0x2000, data);
	}

	/* everything else is ROM and we ignore it */
}



/*************************************
 *
 *  Audio board I/O reads
 *
 *************************************/

READ8_MEMBER(exidy440_state::exidy440_input_port_3_r)
{
	/* I/O1 accesses clear the CIRQ flip/flop */
	m_maincpu->set_input_line(0, CLEAR_LINE);
	return ioport("IN3")->read();
}


READ8_MEMBER(exidy440_state::sound_command_ack_r)
{
	/* sound command acknowledgements come on bit 3 here */
	return m_custom->exidy440_sound_command_ack() ? 0xf7 : 0xff;
}



/*************************************
 *
 *  Audio board I/O writes
 *
 *************************************/

TIMER_CALLBACK_MEMBER(exidy440_state::delayed_sound_command_w)
{
	m_custom->exidy440_sound_command(param);
}


WRITE8_MEMBER(exidy440_state::sound_command_w)
{
	machine().scheduler().synchronize(timer_expired_delegate(FUNC(exidy440_state::delayed_sound_command_w),this), data);
}


WRITE8_MEMBER(exidy440_state::exidy440_input_port_3_w)
{
	/* I/O1 accesses clear the CIRQ flip/flop */
	m_maincpu->set_input_line(0, CLEAR_LINE);
}


WRITE8_MEMBER(exidy440_state::exidy440_coin_counter_w)
{
	coin_counter_w(machine(), 0, data & 1);
}



/*************************************
 *
 *  Game-specific handlers
 *
 *************************************/

READ8_MEMBER(exidy440_state::showdown_bank0_r)
{
	/* showdown relies on different values from different memory locations */
	/* yukon relies on multiple reads from the same location returning different values */
	UINT8 result = 0xff;

	/* fetch the special data if a bank is selected */
	if (m_showdown_bank_select >= 0)
	{
		result = m_showdown_bank_data[m_showdown_bank_select][m_showdown_bank_offset++];

		/* after 24 bytes, stop and revert back to the beginning */
		if (m_showdown_bank_offset == 0x18)
			m_showdown_bank_offset = 0;
	}

	/* look for special offsets to adjust our behavior */
	if (offset == 0x0055)
		m_showdown_bank_select = -1;
	else if (m_showdown_bank_select == -1)
	{
		m_showdown_bank_select = (offset == 0x00ed) ? 0 : (offset == 0x1243) ? 1 : 0;
		m_showdown_bank_offset = 0;
	}

	return result;
}


READ8_MEMBER(exidy440_state::claypign_protection_r)
{
	return 0x76;
}


READ8_MEMBER(exidy440_state::topsecex_input_port_5_r)
{
	return (ioport("AN1")->read() & 1) ? 0x01 : 0x02;
}


WRITE8_MEMBER(exidy440_state::topsecex_yscroll_w)
{
	*m_topsecex_yscroll = data;
}



/*************************************
 *
 *  Reset
 *
 *************************************/

void exidy440_state::machine_start()
{
	/* the EEROM lives in the uppermost 8k of the top bank */
	UINT8 *rom = memregion("maincpu")->base();

	machine().device<nvram_device>("nvram")->set_base(&rom[0x10000 + 15 * 0x4000 + 0x2000], 0x2000);
}

void exidy440_state::machine_reset()
{
	m_bank = 0xff;
	exidy440_bank_select(0);
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( exidy440_map, AS_PROGRAM, 8, exidy440_state )
	AM_RANGE(0x0000, 0x1fff) AM_RAM AM_SHARE("imageram")
	AM_RANGE(0x2000, 0x209f) AM_RAM_WRITE(exidy440_spriteram_w) AM_SHARE("spriteram")
	AM_RANGE(0x20a0, 0x29ff) AM_RAM
	AM_RANGE(0x2a00, 0x2aff) AM_READWRITE(exidy440_videoram_r, exidy440_videoram_w)
	AM_RANGE(0x2b00, 0x2b00) AM_READ(exidy440_vertical_pos_r)
	AM_RANGE(0x2b01, 0x2b01) AM_READWRITE(exidy440_horizontal_pos_r, exidy440_interrupt_clear_w)
	AM_RANGE(0x2b02, 0x2b02) AM_RAM AM_SHARE("scanline")
	AM_RANGE(0x2b03, 0x2b03) AM_READ_PORT("IN0") AM_WRITE(exidy440_control_w)
	AM_RANGE(0x2c00, 0x2dff) AM_READWRITE(exidy440_paletteram_r, exidy440_paletteram_w)
	AM_RANGE(0x2e00, 0x2e1f) AM_RAM_WRITE(sound_command_w)
	AM_RANGE(0x2e20, 0x2e3f) AM_READWRITE(exidy440_input_port_3_r, exidy440_input_port_3_w)
	AM_RANGE(0x2e40, 0x2e5f) AM_READNOP AM_WRITE(exidy440_coin_counter_w)   /* read: clear coin counters I/O2 */
	AM_RANGE(0x2e60, 0x2e7f) AM_READ_PORT("IN1") AM_WRITENOP
	AM_RANGE(0x2e80, 0x2e9f) AM_READ_PORT("IN2") AM_WRITENOP
	AM_RANGE(0x2ea0, 0x2ebf) AM_READ(sound_command_ack_r) AM_WRITENOP
	AM_RANGE(0x2ec0, 0x2eff) AM_NOP
	AM_RANGE(0x3000, 0x3fff) AM_RAM
	AM_RANGE(0x4000, 0x7fff) AM_READ_BANK("bank1") AM_WRITE(bankram_w)
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

#define COINAGE \
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coinage ) ) \
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0x07, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_2C ) ) \
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x06, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_4C ) ) \
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_3C ) ) \
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_2C ) ) \
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_4C ) ) \
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_4C ) ) \
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )


static INPUT_PORTS_START( crossbow )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( cheyenne )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( combat )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( catch22 )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPSETTING(    0x08, "6" )
	PORT_DIPSETTING(    0x0c, "7" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )
	/* PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN ) */
	/* PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN ) */

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( cracksht )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x00, "Seconds" )
	PORT_DIPSETTING(    0x04, "20" )
	PORT_DIPSETTING(    0x00, "30" )
	PORT_DIPSETTING(    0x08, "40" )
	PORT_DIPSETTING(    0x0c, "50" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( claypign )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( chiller )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x00, "Seconds" )
	PORT_DIPSETTING(    0x04, "30" )
	PORT_DIPSETTING(    0x00, "45" )
	PORT_DIPSETTING(    0x08, "60" )
	PORT_DIPSETTING(    0x0c, "70" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( topsecex )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPSETTING(    0x08, "5" )
	PORT_DIPSETTING(    0x0c, "6" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_REVERSE

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_BUTTON1 )

	PORT_START("IN4")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Fireball")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Laser")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Missile")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Oil")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON7 ) PORT_NAME("Turbo")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON8 ) PORT_NAME("Shield")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 ) PORT_NAME("Top Secret")
INPUT_PORTS_END


static INPUT_PORTS_START( hitnmiss )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,hitnmiss_button1_r, NULL)
	PORT_DIPNAME( 0x0c, 0x00, "Seconds" )
	PORT_DIPSETTING(    0x04, "20" )
	PORT_DIPSETTING(    0x00, "30" )
	PORT_DIPSETTING(    0x08, "40" )
	PORT_DIPSETTING(    0x0c, "50" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)

	PORT_START("HITNMISS_BUTTON1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
INPUT_PORTS_END


static INPUT_PORTS_START( whodunit )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x04, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x0c, "5" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( showdown )
	PORT_START("IN0")       /* player inputs and logic board dips */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x0c, 0x00, "Hands" )
	PORT_DIPSETTING(    0x04, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x08, "3" )
	PORT_DIPSETTING(    0x0c, "4" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_beam_r, NULL)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, exidy440_state,firq_vblank_r, NULL)

	PORT_START("IN1")       /* audio board dips */
	COINAGE
	PORT_DIPNAME( 0x70, 0x70, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x70, "1" )
	PORT_DIPSETTING(    0x60, "2" )
	PORT_DIPSETTING(    0x50, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPSETTING(    0x30, "5" )
	PORT_DIPSETTING(    0x20, "6" )
	PORT_DIPSETTING(    0x10, "7" )
	PORT_DIPSETTING(    0x00, "1/Freeplay" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")       /* start button */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 ) PORT_NAME("Action")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Bet-All")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Gold")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Blue")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_NAME("Red")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON8 ) PORT_NAME("White")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN3")       /* coin counters */
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, exidy440_state,coin_inserted, 0)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW,  IPT_UNKNOWN )

	PORT_START("AN0")       /* fake analog X */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10)

	PORT_START("AN1")       /* fake analog Y */
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(10)
INPUT_PORTS_END



/*************************************
 *
 *  Machine drivers
 *
 *************************************/

static MACHINE_CONFIG_START( exidy440, exidy440_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6809, MAIN_CPU_CLOCK)
	MCFG_CPU_PROGRAM_MAP(exidy440_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", exidy440_state,  exidy440_vblank_interrupt)

	MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	MCFG_FRAGMENT_ADD(exidy440_video)

	/* audio hardware */
	MCFG_FRAGMENT_ADD(exidy440_audio)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( topsecex, exidy440 )

	/* basic machine hardware */

	/* video hardware */
	MCFG_FRAGMENT_ADD(topsecex_video)
MACHINE_CONFIG_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( crossbow )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "xbl-2.1a",   0x08000, 0x2000, CRC(bd53ac46) SHA1(0f16ff2d5d08b20c8388d9334995b21c455bf155) )
	ROM_LOAD( "xbl-2.3a",   0x0a000, 0x2000, CRC(703e1376) SHA1(602fc4d9c8fdf3a56ff83112f1e4e8aa5879da8c) )
	ROM_LOAD( "xbl-2.4a",   0x0c000, 0x2000, CRC(52c5daa1) SHA1(eee39da9057dbe855bcabfe32c3ea24609b734f5) )
	ROM_LOAD( "xbl-2.6a",   0x0e000, 0x2000, CRC(f42a68f7) SHA1(1e05e10ece50b77c383b7b36a2eecb7fb5b39de4) )
	ROM_LOAD( "xbl-1.1e",   0x10000, 0x2000, CRC(2834258e) SHA1(f0974c30099a295a9c000038d91caff418b3b1b1) )
	ROM_LOAD( "xbl-1.3e",   0x12000, 0x2000, CRC(587b186c) SHA1(b7d4e1535aeb32c3ce02673be81746759d47b53c) )
	ROM_LOAD( "xbl-1.4e",   0x14000, 0x2000, CRC(23fbfa8e) SHA1(fb6ab1676167d246af51271f7da025f7828bcedb) )
	ROM_LOAD( "xbl-1.6e",   0x16000, 0x2000, CRC(a3ebcc92) SHA1(837b6928df1feddbde27c162b6f3d439d6f5b4b9) )
	ROM_LOAD( "xbl-1.7e",   0x18000, 0x2000, CRC(945b3a68) SHA1(2feebc8ad9805e9fe631b8ebcf400983b03e6822) )
	ROM_LOAD( "xbl-1.8e",   0x1a000, 0x2000, CRC(0d1c5d24) SHA1(e4585c6c97db1ad8ababe310e49b8b6f424bd6da) )
	ROM_LOAD( "xbl-1.10e",  0x1c000, 0x2000, CRC(ca30788b) SHA1(9b32c072fd96b52f268733ea59c3a39ada100ab3) )
	ROM_LOAD( "xbl-1.11e",  0x1e000, 0x2000, CRC(6661c5ee) SHA1(abf535e8045fccccd203733351efdced6445c2d9) )
	ROM_LOAD( "xbl-1.1d",   0x20000, 0x2000, CRC(a1416191) SHA1(0db0be77e430cb165bf82fbdd8c67e3100ae06ba) )
	ROM_LOAD( "xbl-1.3d",   0x22000, 0x2000, CRC(7322b5e1) SHA1(e471a04892d069d16a40ab95c2d623dcb0cbaa3f) )
	ROM_LOAD( "xbl-1.4d",   0x24000, 0x2000, CRC(425d51ef) SHA1(4a58eb21004b79fe361545552b7c29dca4785463) )
	ROM_LOAD( "xbl-1.6d",   0x26000, 0x2000, CRC(c923c9f5) SHA1(704fb7bc648c99c0bfda631239b301dbe10b441d) )
	ROM_LOAD( "xbl-1.7d",   0x28000, 0x2000, CRC(46cdf117) SHA1(15ab85500af5e46024fae14e6d2cdd4cf9a7fa8a) )
	ROM_LOAD( "xbl-1.8d",   0x2a000, 0x2000, CRC(62bad9b6) SHA1(37b3142d96c61dfe8e54baf35144368361da09c6) )
	ROM_LOAD( "xbl-1.10d",  0x2c000, 0x2000, CRC(d4aaa382) SHA1(168a5ecc173eb80078ed0cc9e7fc48e996facac4) )
	ROM_LOAD( "xbl-1.11d",  0x2e000, 0x2000, CRC(efc77790) SHA1(4d3aa33eed042290e8a426d460b8947fe79e24ea) )
	ROM_LOAD( "xbl-1.1c",   0x30000, 0x2000, CRC(dbbd35cb) SHA1(2a13e1d26e4bf9d37234e695f222d665c0ce71e0) )
	ROM_LOAD( "xbl-1.3c",   0x32000, 0x2000, CRC(f011f98d) SHA1(90b5212822e5d5f5f681d85fad7adaff9297f576) )
	ROM_LOAD( "xbl-1.4c",   0x34000, 0x2000, CRC(1257b947) SHA1(4f0e81f24ce4caf93fde12015f86beaa98ab81f2) )
	ROM_LOAD( "xbl-1.6c",   0x36000, 0x2000, CRC(48da9081) SHA1(de2f96643602481b63f6d8dd79afeaea4fc0297a) )
	ROM_LOAD( "xbl-1.7c",   0x38000, 0x2000, CRC(8d4d4855) SHA1(95f2e5d4369bea7ba14d3cf56cfc975b73964858) )
	ROM_LOAD( "xbl-1.8c",   0x3a000, 0x2000, CRC(4c52b85a) SHA1(ab6c827b5da34ce14980874892261bcf1763d23a) )
	ROM_LOAD( "xbl-1.10c",  0x3c000, 0x2000, CRC(5986130b) SHA1(65ed5302523cf79e62c878798a38835b92970a8f) )
	ROM_LOAD( "xbl-1.11c",  0x3e000, 0x2000, CRC(163a6ae0) SHA1(629f84ba80117ea7e4fa73a998bbe69df316d635) )
	ROM_LOAD( "xbl-1.1b",   0x40000, 0x2000, CRC(36ea0269) SHA1(f30181e4c9d37d927b654fede8ddf5cbf430f317) )
	ROM_LOAD( "xbl-1.3b",   0x42000, 0x2000, CRC(4a03c2c9) SHA1(dd60cd629f60d15dd0596bde44fea4b4f1d65ae2) )
	ROM_LOAD( "xbl-1.4b",   0x44000, 0x2000, CRC(7e21c624) SHA1(9e0c1297413f9d440106f6cef25f48fad60e4c85) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "xba-11.1h",  0x0e000, 0x2000, CRC(1b61d0c1) SHA1(de1028a3295dc0413756d4751ca577a03431583e) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "xba-1.2k",   0x00000, 0x2000, CRC(b6e57685) SHA1(ee690cb966af126bfb0bafa804e0ad5490cab1db) )
	ROM_LOAD( "xba-1.2l",   0x02000, 0x2000, CRC(2c24cb35) SHA1(4ea16998f477d6429a92ca05ef74daa21315e695) )
	ROM_LOAD( "xba-1.2m",   0x04000, 0x2000, CRC(f3a4f2be) SHA1(f0ab8a0a6fbb2911d99c961a65035835e54924de) )
	ROM_LOAD( "xba-1.2n",   0x06000, 0x2000, CRC(15cf362d) SHA1(e6ba3766f4f5efb207a6af685cfcdb3c01d41b73) )
	ROM_LOAD( "xba-1.2p",   0x08000, 0x2000, CRC(56f53af9) SHA1(86b5866f76120e72674c565db44fd9bdf387c41a) )
	ROM_LOAD( "xba-1.2r",   0x0a000, 0x2000, CRC(3d8277b0) SHA1(5e65b80801c0d5568909f6f956eda45feb69a727) )
	ROM_LOAD( "xba-1.2s",   0x0c000, 0x2000, CRC(14dd8993) SHA1(066e163fca6d8f696d98d78b41b54a8d06eaba47) )
	ROM_LOAD( "xba-1.2t",   0x0e000, 0x2000, CRC(dfa783e4) SHA1(cb5e1132224085ff9694b8037abbed8d0926ca99) )
	ROM_LOAD( "xba-1.1k",   0x10000, 0x2000, CRC(4f01f9e6) SHA1(2cc75f1e714eef6e315707ccfde39827a057a674) )
	ROM_LOAD( "xba-1.1l",   0x12000, 0x2000, CRC(fb119acf) SHA1(dec0eb4ae33aca6d0dd47f64337c119eb0601e41) )
	ROM_LOAD( "xba-1.1m",   0x14000, 0x2000, CRC(18d097ac) SHA1(c3546c5a21458e7117d36f2e477d3d5db7827487) )
	ROM_LOAD( "xba-1.1n",   0x16000, 0x2000, CRC(2e855698) SHA1(fa4c3ec03fdd1c569c0ca2418899ffa81b5259ec) )
	ROM_LOAD( "xba-1.1p",   0x18000, 0x2000, CRC(788bfac6) SHA1(8cec8ea7a876939719e9901b00055fc90615f237) )
	ROM_LOAD( "xba-1.1r",   0x1a000, 0x2000, CRC(b8ec43b3) SHA1(aebc8151025104d45d9b0ca88118f12b7ef9564b) )
	ROM_LOAD( "xba-1.1s",   0x1c000, 0x2000, CRC(c9ead134) SHA1(411115da95eb42d14a60a7f10c660038cc882005) )
	ROM_LOAD( "xba-1.1t",   0x1e000, 0x2000, CRC(5f41c282) SHA1(670b94534051ce5c6f0c8e0ff5ad7ab78c95be19) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( cheyenne )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "cyl-1.1a",   0x08000, 0x2000, CRC(504c3fa6) SHA1(e3b8b3cfb0884c8bc7871382c3a2bf7dcd70641d) )
	ROM_LOAD( "cyl-1.3a",   0x0a000, 0x2000, CRC(09b7903b) SHA1(24676bf3f01a451b502281321828c80f875b5427) )
	ROM_LOAD( "cyl-1.4a",   0x0c000, 0x2000, CRC(b708646b) SHA1(790a04c2b7dce6577e3fa9625765e121bcc11acf) )
	ROM_LOAD( "cyl-1.6a",   0x0e000, 0x2000, CRC(5d1e708d) SHA1(57a067234701e814d96db2f5216acd9cd8aa8d2c) )
	ROM_LOAD( "cyl-1.1e",   0x10000, 0x2000, CRC(8778e317) SHA1(bcec069b50809fd14c022e98cffa5db858adfa18) )
	ROM_LOAD( "cyl-1.3e",   0x12000, 0x2000, CRC(c8a9ca1b) SHA1(9d9102233029692feeada5b2e0890ca6d2eb1d87) )
	ROM_LOAD( "cyl-1.4e",   0x14000, 0x2000, CRC(86c4125a) SHA1(fb3c866cd4fa9a3d5dac052629f5085679208ef4) )
	ROM_LOAD( "cyl-1.6e",   0x16000, 0x2000, CRC(51f4f060) SHA1(efdab96a81389f212a1ce1354d255a93066c4d0b) )
	ROM_LOAD( "cyl-1.7e",   0x18000, 0x2000, CRC(4924d0c1) SHA1(d2d4c70252da479b25931ce3de1966feb8e2540a) )
	ROM_LOAD( "cyl-1.8e",   0x1a000, 0x2000, CRC(5c7c4dd7) SHA1(958248dbac2718ccbb241612c4a686c323c304a7) )
	ROM_LOAD( "cyl-1.10e",  0x1c000, 0x2000, CRC(57232888) SHA1(e8ca135814916cb11b7196c35054ae63a75b42ad) )
	ROM_LOAD( "cyl-1.11e",  0x1e000, 0x2000, CRC(2a767252) SHA1(11e32a1c9e7d201d016b4773f4a68655fe02a1c0) )
	ROM_LOAD( "cyl-1.1d",   0x20000, 0x2000, CRC(cd590e99) SHA1(d0e9cf44e813798a42990811f4275f435dd46ffa) )
	ROM_LOAD( "cyl-1.3d",   0x22000, 0x2000, CRC(1fddccdb) SHA1(403fc0dbdd6cc0faf6d884dd785fe3eb26834c19) )
	ROM_LOAD( "cyl-1.4d",   0x24000, 0x2000, CRC(6c5ee6d7) SHA1(ad5497bec7c82f79d2548202673e7153e4da0514) )
	ROM_LOAD( "cyl-1.6d",   0x26000, 0x2000, CRC(0e7c16c2) SHA1(a8973d79e15057624fd8b4e3a5824b0fdfb66a23) )
	ROM_LOAD( "cyl-1.7d",   0x28000, 0x2000, CRC(abe11728) SHA1(e27b009d816a03103ea624c382d844c8fc78c3e3) )
	ROM_LOAD( "cyl-1.8d",   0x2a000, 0x2000, CRC(95bb9a72) SHA1(bdf0f9e3adc1c7e9143a1fe1955c8b63edff54b0) )
	ROM_LOAD( "cyl-1.10d",  0x2c000, 0x2000, CRC(5bc251be) SHA1(f9614ddb1a89711673665a2c807b242887e1fa26) )
	ROM_LOAD( "cyl-1.7c",   0x38000, 0x2000, CRC(e9f6ce96) SHA1(e17f5799f9065ad4bd97755f6676dc7f9976d85d) )
	ROM_LOAD( "cyl-1.8c",   0x3a000, 0x2000, CRC(cb3f8e9e) SHA1(7bba735cbed07e64a6c72d06d6f8948e1a0fb863) )
	ROM_LOAD( "cyl-1.10c",  0x3c000, 0x2000, CRC(49f90633) SHA1(6947555e322af0c8f2558b1a018118eed7aa2a2b) )
	ROM_LOAD( "cyl-1.11c",  0x3e000, 0x2000, CRC(70b69cf1) SHA1(9c151888afc4eee8d5ea48f3d4ed517a55e56b44) )
	ROM_LOAD( "cyl-1.1b",   0x40000, 0x2000, CRC(c372e018) SHA1(c7dc083c902b8e0e4212f6853b891bc53b7f2ca6) )
	ROM_LOAD( "cyl-1.3b",   0x42000, 0x2000, CRC(6a583feb) SHA1(8ff5908718e8b341462a3df3e9793ad0b7bbd7f7) )
	ROM_LOAD( "cyl-1.4b",   0x44000, 0x2000, CRC(670e127d) SHA1(c6e8222eb401c868896af4f856520ad3b04cb994) )
	ROM_LOAD( "cyl-1.6b",   0x46000, 0x2000, CRC(ed245268) SHA1(a2ff477f7ac29d280495beeef71c02982e0bf90f) )
	ROM_LOAD( "cyl-1.7b",   0x48000, 0x2000, CRC(dcc56d6c) SHA1(375ba5c765bb5ec3212d7a30c4db8e33e93067b6) )
	ROM_LOAD( "cyl-1.8b",   0x4a000, 0x2000, CRC(c0653d3e) SHA1(489e61d1e0a18fca47b906d80b88c47fdb927d36) )
	ROM_LOAD( "cyl-1.10b",  0x4c000, 0x2000, CRC(7fc67d19) SHA1(48307d50066c02376522e8fee0298c16f758b61d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "cya-1.1h",   0x0e000, 0x2000, CRC(5aed3d8c) SHA1(d04ddd09df471cd2a8dd87c47c7b55eca5d7ac15) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "cya-1.2k",   0x00000, 0x2000, CRC(dc2b716d) SHA1(ae588954431f8c4ecc7075f4964c3b8cc7ae0569) )
	ROM_LOAD( "cya-1.2l",   0x02000, 0x2000, CRC(091ad047) SHA1(edab4472f39a1f19614737c6c5722677f4afd68c) )
	ROM_LOAD( "cya-1.2m",   0x04000, 0x2000, CRC(59085362) SHA1(d4d7182ccdec17a29c556810b1d24aa6726f3826) )
	ROM_LOAD( "cya-1.2n",   0x06000, 0x2000, CRC(9c2e23c7) SHA1(4669c4949ebe69be3a20586a005716e84e0f3ac3) )
	ROM_LOAD( "cya-1.2p",   0x08000, 0x2000, CRC(eff18766) SHA1(623bd0e1bf645c683e77443bd256655ffc6ef2c2) )
	ROM_LOAD( "cya-1.2r",   0x0a000, 0x2000, CRC(8e730c98) SHA1(ab1dcbc5275da92d43abe4d88dcb7df8d21d24be) )
	ROM_LOAD( "cya-1.2s",   0x0c000, 0x2000, CRC(46515454) SHA1(85034df427134462459604dbf1ba6b3369538e12) )
	ROM_LOAD( "cya-1.2t",   0x0e000, 0x2000, CRC(5868fa84) SHA1(b9e71ac94cb4d63af8195991d486ae494ced1ed4) )
	ROM_LOAD( "cya-1.1k",   0x10000, 0x2000, CRC(45a306a6) SHA1(471b6329bb83ca14d3320f7a729f3a83557bb075) )
	ROM_LOAD( "cya-1.1l",   0x12000, 0x2000, CRC(3c7e2127) SHA1(85646e670e3415192ffec7bd524c4033f6bba25f) )
	ROM_LOAD( "cya-1.1m",   0x14000, 0x2000, CRC(39ddc9f7) SHA1(5230b595009dc6020a181755cbced6f0db11f2d2) )
	ROM_LOAD( "cya-1.1n",   0x16000, 0x2000, CRC(5fcee4fd) SHA1(3e4bc94284db237e5b0a5966cb1d2774f030d4b5) )
	ROM_LOAD( "cya-1.1p",   0x18000, 0x2000, CRC(81a4a876) SHA1(7dc18b0af14e11c9f14d6b03fd68709fc4d4d63f) )
	ROM_LOAD( "cya-1.1r",   0x1a000, 0x2000, CRC(dfd84e73) SHA1(6974091ce5cb1677d2e91da263be443023e55ce8) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( combat )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "1a",   0x08000, 0x2000, CRC(159a573b) SHA1(751614f0c9518f6f55e647daa4a7bbf795bc6e6d) )
	ROM_LOAD( "3a",   0x0a000, 0x2000, CRC(59ae51a7) SHA1(c964aefb0705af0966386bae20fd5f885d1677b7) )
	ROM_LOAD( "4a",   0x0c000, 0x2000, CRC(95a1f3d0) SHA1(08f27ae7aa9959f22f08cbfdb101d31ce7e1510b) )
	ROM_LOAD( "6a",   0x0e000, 0x2000, CRC(af3fef5f) SHA1(f07d7313c053b7278ac640a0e5487700aa66f028) )
	ROM_LOAD( "6d",   0x26000, 0x2000, CRC(43d3eb61) SHA1(cc081cef28d930ec7c0de5b009d611673c4e4623) )
	ROM_LOAD( "7d",   0x28000, 0x2000, CRC(ef31659c) SHA1(254a2c41ff28bf64f7e4d55a04602a55f936025e) )
	ROM_LOAD( "8d",   0x2a000, 0x2000, CRC(fb29c5cd) SHA1(72e9d235031e523d35deae9dfc5215e1828c4f8b) )
	ROM_LOAD( "10d",  0x2c000, 0x2000, CRC(2ca0eaa4) SHA1(600d6de569e68a268fb2e03375e852ca9c792fe5) )
	ROM_LOAD( "11d",  0x2e000, 0x2000, CRC(cc9f2001) SHA1(2acc4021f22b0c2e95975ded4a545e8bb0780da5) )
	ROM_LOAD( "1c",   0x30000, 0x2000, CRC(b7b9c5ad) SHA1(c55d63420d7d50d8230a108c1d7716d6fbd8e29f) )
	ROM_LOAD( "3c",   0x32000, 0x2000, CRC(b700e6ec) SHA1(0d4876fa2ed813c031e633cf0ed78e301f91a6f1) )
	ROM_LOAD( "4c",   0x34000, 0x2000, CRC(89fc2b2d) SHA1(ef404ddbd04a19d25b6c8e609e1911472b733379) )
	ROM_LOAD( "6c",   0x36000, 0x2000, CRC(6a8d0dcf) SHA1(9b060e96dbba652a5ef698e44ae94cf0fb750310) )
	ROM_LOAD( "7c",   0x38000, 0x2000, CRC(9df7172d) SHA1(59978b7c8feab0af1c230fbc4568ce587d1e7e07) )
	ROM_LOAD( "8c",   0x3a000, 0x2000, CRC(63b2e4f3) SHA1(dc306f6a16a8fc51f9ad62f0314af80b1da28782) )
	ROM_LOAD( "10c",  0x3c000, 0x2000, CRC(3b430adc) SHA1(2e2f08b372e85a16a62bc6ab59e6f5c17366dde0) )
	ROM_LOAD( "11c",  0x3e000, 0x2000, CRC(04301032) SHA1(bed498e9e693ca121df633e3b2329c9dffb41cf9) )
	ROM_LOAD( "1b",   0x40000, 0x2000, CRC(70e25cae) SHA1(c6257b6548c89ce366c87efc04bc9e6571329e80) )
	ROM_LOAD( "3b",   0x42000, 0x2000, CRC(d09d167e) SHA1(b5f1e83b31727b15c8668a373cf503fb50de462e) )
	ROM_LOAD( "4b",   0x44000, 0x2000, CRC(f46aba0d) SHA1(6bc4e204a6ff6bf31b1a319190a1b47642e1c54c) )
	ROM_LOAD( "6b",   0x46000, 0x2000, CRC(8eb46f40) SHA1(93553fad23ee8183777875b35f7e0b3d67ed9ec9) )
	ROM_LOAD( "7b",   0x48000, 0x2000, CRC(3be9b1bd) SHA1(7852cbccc14eacb2980cd9e496e9a6ebba0b06b1) )
	ROM_LOAD( "8b",   0x4a000, 0x2000, CRC(ae977f4c) SHA1(a4cc9cae10482f03879b64c2b40fc8999b8a2b71) )
	ROM_LOAD( "10b",  0x4c000, 0x2000, CRC(502da003) SHA1(f4c579b2f997208f71b24590794275d87b06e25c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1h",  0x0e000, 0x2000, CRC(8f3dd350) SHA1(9e329c2f502f63fcdbebeb40bf732e4a07a463c1) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "2k",          0x00000, 0x2000, CRC(1c9df8b5) SHA1(12e82f585aee01f1e2ea7396a6b013c894f7b98d) )
	ROM_LOAD( "2l",          0x02000, 0x2000, CRC(6b733306) SHA1(a41cc2e646392d71642abe2ab8d72f2d56214c02) )
	ROM_LOAD( "2m",          0x04000, 0x2000, CRC(dc074733) SHA1(29a036d4057b813f584373493cb5b69b711840ae) )
	ROM_LOAD( "2n",          0x06000, 0x2000, CRC(7985867f) SHA1(8d86777b7afa8a6c1a36e598a83feeb28536f02e) )
	ROM_LOAD( "2p",          0x08000, 0x2000, CRC(88684dcf) SHA1(fa006db1d70d2b557ab867ad766e27374e252f8c) )
	ROM_LOAD( "2r",          0x0a000, 0x2000, CRC(5857321e) SHA1(9726ce74b0e0b3e7fa44002d42342c5f2be00c22) )
	ROM_LOAD( "2s",          0x0c000, 0x2000, CRC(371e5235) SHA1(6cfa5ba2715a33c61fadc3d5b9347c26bb3279dd) )
	ROM_LOAD( "2t",          0x0e000, 0x2000, CRC(7ae65f05) SHA1(404bcfe629aecf1e0835d7feeaff8654829fd10f) )
	ROM_LOAD( "1k",      0x10000, 0x2000, CRC(f748ea87) SHA1(4d90f44edb01d65c28c9742c50b4cbe6e26aced3) )
	ROM_LOAD( "xba-1.2s", 0x16000, 0x2000, CRC(14dd8993) SHA1(066e163fca6d8f696d98d78b41b54a8d06eaba47) )   /* from Crossbow */
	ROM_LOAD( "xba-1.1n", 0x18000, 0x2000, CRC(2e855698) SHA1(fa4c3ec03fdd1c569c0ca2418899ffa81b5259ec) )   /* from Crossbow */
	ROM_LOAD( "xba-1.1p", 0x1a000, 0x2000, CRC(788bfac6) SHA1(8cec8ea7a876939719e9901b00055fc90615f237) )   /* from Crossbow */
	ROM_LOAD( "xba-1.2l", 0x1c000, 0x2000, CRC(2c24cb35) SHA1(4ea16998f477d6429a92ca05ef74daa21315e695) )   /* from Crossbow */
	ROM_LOAD( "xba-1.1t", 0x1e000, 0x2000, CRC(5f41c282) SHA1(670b94534051ce5c6f0c8e0ff5ad7ab78c95be19) )   /* from Crossbow */

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( catch22 )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "22l-8_0.1a",   0x08000, 0x2000, CRC(232e8723) SHA1(10da33e94026dc856ea4fcdd11945527321383d1) )
	ROM_LOAD( "22l-8_0.3a",   0x0a000, 0x2000, CRC(a94afce6) SHA1(bbc349240460c001e8eda78d80b36bb2c40b090e) )
	ROM_LOAD( "22l-8_0.4a",   0x0c000, 0x2000, CRC(0983ab83) SHA1(9954400f4595fdd6633b2b108a6d64bff24e7f23) )
	ROM_LOAD( "22l-8_0.6a",   0x0e000, 0x2000, CRC(9084a23a) SHA1(fbe9020d5d55dd7aec8958dccfa8931c9d950bc2) )
	ROM_LOAD( "6d",   0x26000, 0x2000, CRC(43d3eb61) SHA1(cc081cef28d930ec7c0de5b009d611673c4e4623) )
	ROM_LOAD( "7d",   0x28000, 0x2000, CRC(ef31659c) SHA1(254a2c41ff28bf64f7e4d55a04602a55f936025e) )
	ROM_LOAD( "8d",   0x2a000, 0x2000, CRC(fb29c5cd) SHA1(72e9d235031e523d35deae9dfc5215e1828c4f8b) )
	ROM_LOAD( "10d",  0x2c000, 0x2000, CRC(2ca0eaa4) SHA1(600d6de569e68a268fb2e03375e852ca9c792fe5) )
	ROM_LOAD( "11d",  0x2e000, 0x2000, CRC(cc9f2001) SHA1(2acc4021f22b0c2e95975ded4a545e8bb0780da5) )
	ROM_LOAD( "1c",   0x30000, 0x2000, CRC(b7b9c5ad) SHA1(c55d63420d7d50d8230a108c1d7716d6fbd8e29f) )
	ROM_LOAD( "3c",   0x32000, 0x2000, CRC(b700e6ec) SHA1(0d4876fa2ed813c031e633cf0ed78e301f91a6f1) )
	ROM_LOAD( "4c",   0x34000, 0x2000, CRC(89fc2b2d) SHA1(ef404ddbd04a19d25b6c8e609e1911472b733379) )
	ROM_LOAD( "6c",   0x36000, 0x2000, CRC(6a8d0dcf) SHA1(9b060e96dbba652a5ef698e44ae94cf0fb750310) )
	ROM_LOAD( "7c",   0x38000, 0x2000, CRC(9df7172d) SHA1(59978b7c8feab0af1c230fbc4568ce587d1e7e07) )
	ROM_LOAD( "8c",   0x3a000, 0x2000, CRC(63b2e4f3) SHA1(dc306f6a16a8fc51f9ad62f0314af80b1da28782) )
	ROM_LOAD( "10c",  0x3c000, 0x2000, CRC(3b430adc) SHA1(2e2f08b372e85a16a62bc6ab59e6f5c17366dde0) )
	ROM_LOAD( "11c",  0x3e000, 0x2000, CRC(04301032) SHA1(bed498e9e693ca121df633e3b2329c9dffb41cf9) )
	ROM_LOAD( "1b",   0x40000, 0x2000, CRC(70e25cae) SHA1(c6257b6548c89ce366c87efc04bc9e6571329e80) )
	ROM_LOAD( "3b",   0x42000, 0x2000, CRC(d09d167e) SHA1(b5f1e83b31727b15c8668a373cf503fb50de462e) )
	ROM_LOAD( "4b",   0x44000, 0x2000, CRC(f46aba0d) SHA1(6bc4e204a6ff6bf31b1a319190a1b47642e1c54c) )
	ROM_LOAD( "6b",   0x46000, 0x2000, CRC(8eb46f40) SHA1(93553fad23ee8183777875b35f7e0b3d67ed9ec9) )
	ROM_LOAD( "7b",   0x48000, 0x2000, CRC(3be9b1bd) SHA1(7852cbccc14eacb2980cd9e496e9a6ebba0b06b1) )
	ROM_LOAD( "8b",   0x4a000, 0x2000, CRC(ae977f4c) SHA1(a4cc9cae10482f03879b64c2b40fc8999b8a2b71) )
	ROM_LOAD( "10b",  0x4c000, 0x2000, CRC(502da003) SHA1(f4c579b2f997208f71b24590794275d87b06e25c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1h",  0x0e000, 0x2000, CRC(8f3dd350) SHA1(9e329c2f502f63fcdbebeb40bf732e4a07a463c1) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "2k",          0x00000, 0x2000, CRC(1c9df8b5) SHA1(12e82f585aee01f1e2ea7396a6b013c894f7b98d) )
	ROM_LOAD( "2l",          0x02000, 0x2000, CRC(6b733306) SHA1(a41cc2e646392d71642abe2ab8d72f2d56214c02) )
	ROM_LOAD( "2m",          0x04000, 0x2000, CRC(dc074733) SHA1(29a036d4057b813f584373493cb5b69b711840ae) )
	ROM_LOAD( "2n",          0x06000, 0x2000, CRC(7985867f) SHA1(8d86777b7afa8a6c1a36e598a83feeb28536f02e) )
	ROM_LOAD( "2p",          0x08000, 0x2000, CRC(88684dcf) SHA1(fa006db1d70d2b557ab867ad766e27374e252f8c) )
	ROM_LOAD( "2r",          0x0a000, 0x2000, CRC(5857321e) SHA1(9726ce74b0e0b3e7fa44002d42342c5f2be00c22) )
	ROM_LOAD( "2s",          0x0c000, 0x2000, CRC(371e5235) SHA1(6cfa5ba2715a33c61fadc3d5b9347c26bb3279dd) )
	ROM_LOAD( "2t",          0x0e000, 0x2000, CRC(7ae65f05) SHA1(404bcfe629aecf1e0835d7feeaff8654829fd10f) )
	ROM_LOAD( "1k",      0x10000, 0x2000, CRC(f748ea87) SHA1(4d90f44edb01d65c28c9742c50b4cbe6e26aced3) )
	ROM_LOAD( "xba-1.2s", 0x16000, 0x2000, CRC(14dd8993) SHA1(066e163fca6d8f696d98d78b41b54a8d06eaba47) )   /* from Crossbow */
	ROM_LOAD( "xba-1.1n", 0x18000, 0x2000, CRC(2e855698) SHA1(fa4c3ec03fdd1c569c0ca2418899ffa81b5259ec) )   /* from Crossbow */
	ROM_LOAD( "xba-1.1p", 0x1a000, 0x2000, CRC(788bfac6) SHA1(8cec8ea7a876939719e9901b00055fc90615f237) )   /* from Crossbow */
	ROM_LOAD( "xba-1.2l", 0x1c000, 0x2000, CRC(2c24cb35) SHA1(4ea16998f477d6429a92ca05ef74daa21315e695) )   /* from Crossbow */
	ROM_LOAD( "xba-1.1t", 0x1e000, 0x2000, CRC(5f41c282) SHA1(670b94534051ce5c6f0c8e0ff5ad7ab78c95be19) )   /* from Crossbow */

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( cracksht )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "csl2.1a",   0x08000, 0x2000, CRC(16fd0171) SHA1(3c39d8e3483e0d6b82ec2e0d3cdd61250015b165) )
	ROM_LOAD( "csl2.3a",   0x0a000, 0x2000, CRC(906f3209) SHA1(2c479e525a3ed31969c168ddf5a72dc909c2c494) )
	ROM_LOAD( "csl2.4a",   0x0c000, 0x2000, CRC(9996d2bf) SHA1(5548cc3f83d745efa4d07fd36e1772b1eae5de48) )
	ROM_LOAD( "csl2.6a",   0x0e000, 0x2000, CRC(c8d6e945) SHA1(38f271730b6a652bad744c4f7221276b860a4f84) )
	ROM_LOAD( "csl2.11d",  0x2e000, 0x2000, CRC(b1173dd3) SHA1(b5ee86f980c14e162ccf8af84983f3385d106934) )
	ROM_LOAD( "csl2.1c",   0x30000, 0x2000, CRC(e44975a7) SHA1(b762c5664922dd8648c898a9318e29dac43810d6) )
	ROM_LOAD( "csl2.3c",   0x32000, 0x2000, CRC(a3ab11e9) SHA1(72b1946466027c15bcf7bbc3d21d032e08dde70b) )
	ROM_LOAD( "csl2.4c",   0x34000, 0x2000, CRC(89266302) SHA1(b5b1d20e1db64b51c0567dcf152bc13da2cd2f2c) )
	ROM_LOAD( "csl2.6c",   0x36000, 0x2000, CRC(bb0f8d32) SHA1(7322053747e5b8057051f2e4947c6882bf4bab8c) )
	ROM_LOAD( "csl2.7c",   0x38000, 0x2000, CRC(e203ed0b) SHA1(2117e007c6ee0ed1e52e128e22b70a37e1d87b58) )
	ROM_LOAD( "csl2.8c",   0x3a000, 0x2000, CRC(3e028a62) SHA1(6d5b5f170cfd74457893883fac5085de19f902b2) )
	ROM_LOAD( "csl2.10c",  0x3c000, 0x2000, CRC(c5494f9f) SHA1(bb75a1bcbc20312315f8d50e4a3316ff2b39ec16) )
	ROM_LOAD( "csl2.11c",  0x3e000, 0x2000, CRC(0159bdcb) SHA1(1927f5e3f3eff46b082c24f95bc3fcb107df0a02) )
	ROM_LOAD( "csl2.1b",   0x40000, 0x2000, CRC(8adf33fc) SHA1(7321fe06ca369fc590d6a6992de74cdb5dddbdb5) )
	ROM_LOAD( "csl2.3b",   0x42000, 0x2000, CRC(7561be69) SHA1(204a503cda61723f16e91b46c18aab658eda3941) )
	ROM_LOAD( "csl2.4b",   0x44000, 0x2000, CRC(848e3aff) SHA1(f494ad0c59eaaca9bb9d75998d4a62095bd40c95) )
	ROM_LOAD( "csl2.6b",   0x46000, 0x2000, CRC(d0fd87df) SHA1(3c37c28fe49b41c4b2c3c5a0b384ce2a2d3afd65) )
	ROM_LOAD( "csl2.7b",   0x48000, 0x2000, CRC(7e0a6a31) SHA1(d015926e34ecd9002b17fc8f2e3494cd88ae9176) )
	ROM_LOAD( "csl2.8b",   0x4a000, 0x2000, CRC(af1c8cb8) SHA1(d753539a2afa4f6b0a79b9c7364d9814eb5ec3c0) )
	ROM_LOAD( "csl2.10b",  0x4c000, 0x2000, CRC(8a0d6ad0) SHA1(024a8cebb56404c9efae0594e0b1d4a341ba9893) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "csa3.1h",   0x0e000, 0x2000, CRC(5ba8b4ac) SHA1(04d9d4bb7a5994c5ffe97ca22a43e7a1cbdef559) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "csa3.2k",   0x00000, 0x2000, CRC(067a4f71) SHA1(301b44bcb0c7145dbc2dfbaf5c7d9cc5aa0e2118) )
	ROM_LOAD( "csa3.2l",   0x02000, 0x2000, CRC(5716c59e) SHA1(8adb601ba04bbc27295afe993cdc0576a39c7a71) )
	ROM_LOAD( "csa3.2m",   0x04000, 0x2000, CRC(b3ff659b) SHA1(295b5153ad41d92ee53b53ed454b2487aea7f355) )
	ROM_LOAD( "csa3.2n",   0x06000, 0x2000, CRC(a8968342) SHA1(7c01414bb8ef0e68ff51d3764631cb4614059677) )
	ROM_LOAD( "csa3.2p",   0x08000, 0x2000, CRC(5db225b8) SHA1(f4ab6ac7459777b9e614df9a93d275ab6a47da8b) )
	ROM_LOAD( "csa3.2r",   0x0a000, 0x2000, CRC(fda2669d) SHA1(496cb06418ae04f74a82357cde44ba01bbfe184b) )
	ROM_LOAD( "csa3.2s",   0x0c000, 0x2000, CRC(e8d2413f) SHA1(68a00d788d9dcd251a8cb88eb49103d23d570123) )
	ROM_LOAD( "csa3.2t",   0x0e000, 0x2000, CRC(841a1855) SHA1(9aa01f09116c3256bffa991240abf87ff61b3515) )
	ROM_LOAD( "csa3.1k",   0x10000, 0x2000, CRC(27dda69b) SHA1(b63844df3f387ff269e453c32fded3c987cc7232) )
	ROM_LOAD( "csa3.1l",   0x12000, 0x2000, CRC(86eea479) SHA1(dad2e66f5e7d210ea004f44b65fa9afe5ea748ca) )
	ROM_LOAD( "csa3.1m",   0x14000, 0x2000, CRC(2c24cb35) SHA1(4ea16998f477d6429a92ca05ef74daa21315e695) )
	ROM_LOAD( "csa3.1n",   0x16000, 0x2000, CRC(f3a4f2be) SHA1(f0ab8a0a6fbb2911d99c961a65035835e54924de) )
	ROM_LOAD( "csa3.1p",   0x18000, 0x2000, CRC(14dd8993) SHA1(066e163fca6d8f696d98d78b41b54a8d06eaba47) )
	ROM_LOAD( "csa3.1r",   0x1a000, 0x2000, CRC(dfa783e4) SHA1(cb5e1132224085ff9694b8037abbed8d0926ca99) )
	ROM_LOAD( "csa3.1s",   0x1c000, 0x2000, CRC(18d097ac) SHA1(c3546c5a21458e7117d36f2e477d3d5db7827487) )
	ROM_LOAD( "csa3.1t",   0x1e000, 0x2000, CRC(5f41c282) SHA1(670b94534051ce5c6f0c8e0ff5ad7ab78c95be19) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( claypign )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "claypige.1a",   0x08000, 0x2000, CRC(446d7004) SHA1(c2bbfbfdfd144ff61178dbacd244e2f9d1c3ffc3) )
	ROM_LOAD( "claypige.3a",   0x0a000, 0x2000, CRC(df39701b) SHA1(416f586441e87f7e2b36dee4731249a75b82c897) )
	ROM_LOAD( "claypige.4a",   0x0c000, 0x2000, CRC(f205afb8) SHA1(e031250ac57259dc20aab44d3ab22d4f75c08b9f) )
	ROM_LOAD( "claypige.6a",   0x0e000, 0x2000, CRC(97c36c6c) SHA1(cb9beafe07faf69162f8b691d4dc90149d1a4d28) )
	ROM_LOAD( "claypige.10c",  0x3c000, 0x2000, CRC(3d2957cd) SHA1(3c7c38f94d9e075e18954806ba52c131ff348df8) )
	ROM_LOAD( "claypige.11c",  0x3e000, 0x2000, CRC(e162a3af) SHA1(c7bc6e473f6fc6569a849e0bd135b4ae44aa9e7e) )
	ROM_LOAD( "claypige.1b",   0x40000, 0x2000, CRC(90f1e534) SHA1(26f80eaa0469d94d6e7ab1bae1f45b07697034ef) )
	ROM_LOAD( "claypige.3b",   0x42000, 0x2000, CRC(150c5993) SHA1(a1a0ab71c796a209d109fbabbd8ec0342164618e) )
	ROM_LOAD( "claypige.4b",   0x44000, 0x2000, CRC(dabb99fb) SHA1(491837ad504a16d4ddea9c633a5e8b99dab7870f) )
	ROM_LOAD( "claypige.6b",   0x46000, 0x2000, CRC(c3b86d26) SHA1(df75cb72486a395612575cdc7d5a886465d64c91) )
	ROM_LOAD( "claypige.7b",   0x48000, 0x2000, CRC(6140b026) SHA1(16949d1bcaec3c0c398df50a731da3bb44fa8e5b) )
	ROM_LOAD( "claypige.8b",   0x4a000, 0x2000, CRC(d0f9d170) SHA1(db4285a280a7d539aab91280c57db9c460468a69) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "claypige.h1",   0x0e000, 0x2000, CRC(9eedc68d) SHA1(966542a10e19f7afe065614bdb7dd8a9ad9d3c3d) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "claypige.k2",   0x00000, 0x2000, CRC(0dd93c6c) SHA1(630397dbc54923a713bef1d99b08df8d2668b8ea) )
	ROM_LOAD( "claypige.l2",   0x02000, 0x2000, CRC(e1d67c42) SHA1(8021432493cd9d5096b534505d469bb88a20e31f) )
	ROM_LOAD( "claypige.m2",   0x04000, 0x2000, CRC(b56d8bd5) SHA1(45ac65a0f066791bb50535705d502957bfffbd53) )
	ROM_LOAD( "claypige.n2",   0x06000, 0x2000, CRC(9e381cb5) SHA1(aeedb0030baa8a7f9396b917f0452a3edcd725c2) )
	ROM_LOAD( "xba-1.2l",       0x08000, 0x2000, CRC(2c24cb35) SHA1(4ea16998f477d6429a92ca05ef74daa21315e695) )     /* from Crossbow */
	ROM_LOAD( "xba-1.2k",       0x0a000, 0x2000, CRC(b6e57685) SHA1(ee690cb966af126bfb0bafa804e0ad5490cab1db) )     /* from Crossbow */
	ROM_LOAD( "xba-1.1m",       0x0c000, 0x2000, CRC(18d097ac) SHA1(c3546c5a21458e7117d36f2e477d3d5db7827487) )     /* from Crossbow */
	ROM_LOAD( "xba-1.1t",       0x0e000, 0x2000, CRC(5f41c282) SHA1(670b94534051ce5c6f0c8e0ff5ad7ab78c95be19) )     /* from Crossbow */
	ROM_LOAD( "claypige.k1",   0x10000, 0x2000, CRC(07f12d18) SHA1(57041cd31abfd94f8c5ad172aeafef5302484973) )
	ROM_LOAD( "claypige.l1",   0x12000, 0x2000, CRC(f448eb4f) SHA1(40e2116dcff76a58bc460c4725bfd463d4dda227) )
	ROM_LOAD( "claypige.m1",   0x14000, 0x2000, CRC(36865f5b) SHA1(18dd278760daba466c5a52cf267878ec2afdede7) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( chiller )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "chl3.1a",   0x08000, 0x2000, CRC(996ad02e) SHA1(0c242614f28b8ee1a65a4d336fb645c814a50293) )
	ROM_LOAD( "chl3.3a",   0x0a000, 0x2000, CRC(17e6f904) SHA1(e43adea5f3106bc97ac4762213478a02656300fe) )
	ROM_LOAD( "chl3.4a",   0x0c000, 0x2000, CRC(f30d6e32) SHA1(3425ea4c5a8d840b23626891ac47fc86f612654b) )
	ROM_LOAD( "chl3.6a",   0x0e000, 0x2000, CRC(f64fa8fe) SHA1(7a9dc6d137b6b1849cb108d8a6e2dc51382408b4) )
	ROM_LOAD( "chl3.1d",   0x20000, 0x2000, CRC(ce4aa4b0) SHA1(993e156283f3da80903192740216ac7d3b5bc590) )
	ROM_LOAD( "chl3.3d",   0x22000, 0x2000, CRC(a234952e) SHA1(6b6c2282dfd7db1e057fcd4846b75e0ff8449d6f) )
	ROM_LOAD( "chl3.4d",   0x24000, 0x2000, CRC(645dbae9) SHA1(266a22d7cde66802aeca1a084c3b720660e30293) )
	ROM_LOAD( "chl3.6d",   0x26000, 0x2000, CRC(440a5cd7) SHA1(cbf65278115f9f6b29066938e49edcb71be1cee3) )
	ROM_LOAD( "chl3.7d",   0x28000, 0x2000, CRC(062a541f) SHA1(27d55fc2be5fbb7efb9239bfd9b175c26b2cc82f) )
	ROM_LOAD( "chl3.8d",   0x2a000, 0x2000, CRC(31ff8f48) SHA1(6b43dcc4c9b89e1108e7efae751bb94edbbc8ab2) )
	ROM_LOAD( "chl3.10d",  0x2c000, 0x2000, CRC(5bceb965) SHA1(9691a87cf340d8f034d1ba89dd1b03de9363006e) )
	ROM_LOAD( "chl3.11d",  0x2e000, 0x2000, CRC(e16b5db3) SHA1(4ddbbedd63f02782a3fcec371fe6bc3380c44848) )
	ROM_LOAD( "chl3.1c",   0x30000, 0x2000, CRC(ebfd29e8) SHA1(3927c580e1c079ca60985f9e2c80d0ea9b38eda5) )
	ROM_LOAD( "chl3.3c",   0x32000, 0x2000, CRC(a04261e5) SHA1(de922254c573327423d46b61b52db82afb97d471) )
	ROM_LOAD( "chl3.4c",   0x34000, 0x2000, CRC(6fcbb15b) SHA1(6a2a2a69aaae519aaea7b2379fbd8a68d4be4712) )
	ROM_LOAD( "chl3.6c",   0x36000, 0x2000, CRC(bd0e0689) SHA1(f3feadffc5ce63affb4e620b7869a484cd12190e) )
	ROM_LOAD( "chl3.7c",   0x38000, 0x2000, CRC(2715571e) SHA1(fe053d8c0c203ac7ba579324a5414fac99f4f522) )
	ROM_LOAD( "chl3.8c",   0x3a000, 0x2000, CRC(364d9450) SHA1(d26cb55adf00b65c29b00be87d8aec310c64c345) )
	ROM_LOAD( "chl3.10c",  0x3c000, 0x2000, CRC(13180106) SHA1(c682b387469be12fac53191c46ab25fd42945ef0) )
	ROM_LOAD( "chl3.11c",  0x3e000, 0x2000, CRC(4a7ffe6f) SHA1(73c6bb2aa4c70647d934fc436e1870ad1c93b410) )
	ROM_LOAD( "chl3.1b",   0x40000, 0x2000, CRC(20c19bb6) SHA1(50c44ceb075a0cd301d34db09601014c421b04fe) )
	ROM_LOAD( "chl3.3b",   0x42000, 0x2000, CRC(e1f07ace) SHA1(6f51e289139db759720af912578f068b85e8b894) )
	ROM_LOAD( "chl3.4b",   0x44000, 0x2000, CRC(140d95db) SHA1(cf53963df8d2d4243d1804284e7c6d42c748b81d) )
	ROM_LOAD( "chl3.6b",   0x46000, 0x2000, CRC(faaf7cc8) SHA1(fc18ebc6183fdae43ec92304e356cd792a33d74d) )
	ROM_LOAD( "chl3.7b",   0x48000, 0x2000, CRC(5512b7e6) SHA1(54aa7c9f58cf8350ae59f0d409ac3420a5ad59ba) )
	ROM_LOAD( "chl3.8b",   0x4a000, 0x2000, CRC(6172b12f) SHA1(f23e88103ed6b67eefade835cbdb1e3260d07d92) )
	ROM_LOAD( "chl3.10b",  0x4c000, 0x2000, CRC(5d15342a) SHA1(74216b78a8f0bb44911b9cc74587b3edbacbbf01) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "cha3.1h",   0x0f000, 0x1000, CRC(b195cbba) SHA1(a74d14464ef0f07bfc83500483dd552f38fd55c8) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "cha3.2k",   0x00000, 0x2000, CRC(814a1c6e) SHA1(f7b22bc5a7d0b8fb9914b000520d68cc87c43957) )
	ROM_LOAD( "cha3.2l",   0x02000, 0x2000, CRC(b326007f) SHA1(c636f28f18697673d0a9b47a1494ea4060ca012f) )
	ROM_LOAD( "cha3.2m",   0x04000, 0x2000, CRC(11075e8c) SHA1(f87cb92126ddb3899fc95b3a20a1c7109fc2a60d) )
	ROM_LOAD( "cha3.2n",   0x06000, 0x2000, CRC(8c3f6184) SHA1(c87ce37b3f71cd75b370f2a6f1cd54fd022d65b3) )
	ROM_LOAD( "cha3.2p",   0x08000, 0x2000, CRC(3a8b4d0f) SHA1(d267d0365ce994f02b0ace32a2b440043f516605) )
	ROM_LOAD( "cha3.2r",   0x0a000, 0x2000, CRC(fc6c4e00) SHA1(bddbf016fb43d12aae13b55b1b4e0cb430c3e72e) )
	ROM_LOAD( "cha3.2s",   0x0c000, 0x2000, CRC(2440d5f3) SHA1(9b83e74cb1d0fa690c5a54a081e890e6da21677b) )
	ROM_LOAD( "cha3.2t",   0x0e000, 0x2000, CRC(9b2ce556) SHA1(debecc33c6e6d3861287c4df78279bcfa921ad5c) )
	ROM_LOAD( "cha3.1k",   0x10000, 0x2000, CRC(27f86fab) SHA1(dd8ca294ea338218c85faeeaed0144164203f17e) )
	ROM_LOAD( "cha3.1l",   0x12000, 0x2000, CRC(581dfde7) SHA1(e0bc92fd4900164e9a6bc9424e9a3b991e850ff8) )
	ROM_LOAD( "cha3.1m",   0x14000, 0x2000, CRC(36d47696) SHA1(b4d71daf8b9fe7b501f2e740122eaad08bb03c66) )
	ROM_LOAD( "cha3.1n",   0x16000, 0x2000, CRC(ce47bffe) SHA1(7c7f6402ac54f88e120971f5d5b66858acd15fb3) )
	ROM_LOAD( "cha3.1p",   0x18000, 0x2000, CRC(788bfac6) SHA1(8cec8ea7a876939719e9901b00055fc90615f237) )
	ROM_LOAD( "cha3.1r",   0x1a000, 0x2000, CRC(b8ec43b3) SHA1(aebc8151025104d45d9b0ca88118f12b7ef9564b) )
	ROM_LOAD( "cha3.1s",   0x1c000, 0x2000, CRC(5f41c282) SHA1(670b94534051ce5c6f0c8e0ff5ad7ab78c95be19) )
	ROM_LOAD( "cha3.1t",   0x1e000, 0x2000, CRC(3a3a48af) SHA1(4128f277e12753de66a9a7d5529920b97cfa12a8) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( topsecex )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "tsl1.a1",   0x08000, 0x2000, CRC(30ff2142) SHA1(28dad2a785101229b3de1859f8fb068b49d6037c) )
	ROM_LOAD( "tsl1.a3",   0x0a000, 0x2000, CRC(9295e5b7) SHA1(cc73e6d111f922ca7ecd10ba576df3119cd4d328) )
	ROM_LOAD( "tsl1.a4",   0x0c000, 0x2000, CRC(402abca4) SHA1(02f093d0e81d21bc11e380d90e0ec9ff93a9760b) )
	ROM_LOAD( "tsl1.a6",   0x0e000, 0x2000, CRC(66eac7d8) SHA1(b70f85fe58dceb2add05d06eadd22b398b93129c) )
	ROM_LOAD( "tsl1.e3",   0x12000, 0x2000, CRC(f5b291fc) SHA1(055f4aba52f8e6e0ebda549a6b4a4b3cbf219a49) )
	ROM_LOAD( "tsl1.e4",   0x14000, 0x2000, CRC(b6c659ae) SHA1(cb997ed307c93c6e5b3e2aa5aa10137df8cd09a4) )
	ROM_LOAD( "tsl1.e6",   0x16000, 0x2000, CRC(cf457523) SHA1(0d7e72bfd63ce425a406dbffc315ed2e85b42ace) )
	ROM_LOAD( "tsl1.e7",   0x18000, 0x2000, CRC(5a29304c) SHA1(5edf24c7f2ca6b8e416fe3f5cc856c7b0d9a16a3) )
	ROM_LOAD( "tsl1.e8",   0x1a000, 0x2000, CRC(0750893b) SHA1(b3554858143c43b2e4bda4395c8ccb4a78907237) )
	ROM_LOAD( "tsl1.e10",  0x1c000, 0x2000, CRC(fb87a723) SHA1(ad487d837bdfdf3a8a3668f85a0cbf51f2a729f8) )
	ROM_LOAD( "tsl1.e11",  0x1e000, 0x2000, CRC(ecf78fac) SHA1(acd75d0afa08f419593360fe58fb5e7ac573884a) )
	ROM_LOAD( "tsl1.d1",   0x20000, 0x2000, CRC(3a316cbe) SHA1(27a8875d633631a56f6a43b516b1f0bb14f9a085) )
	ROM_LOAD( "tsl1.d3",   0x22000, 0x2000, CRC(58408a5f) SHA1(5d5bde5b55c5214812188cf76b3986ebb2b4abd5) )
	ROM_LOAD( "tsl1.d4",   0x24000, 0x2000, CRC(c3c85c13) SHA1(4e251030eb754042ed643a6f314cb0a48ab2e4d5) )
	ROM_LOAD( "tsl1.d6",   0x26000, 0x2000, CRC(f26a2864) SHA1(61984a3345dad3226a8a06e2cd8ad65a65aef0c5) )
	ROM_LOAD( "tsl1.d7",   0x28000, 0x2000, CRC(53195dc6) SHA1(8384af556905322b9e06703fa2719951f0552325) )
	ROM_LOAD( "tsl1.d8",   0x2a000, 0x2000, CRC(4fcfb3c8) SHA1(2f865f2c7dc2f5e384173782a41a62170c84fd26) )
	ROM_LOAD( "tsl1.d10",  0x2c000, 0x2000, CRC(6e20af8d) SHA1(a4a00dbe0a850e2df8e1141f2bb9808bc879e765) )
	ROM_LOAD( "tsl1.d11",  0x2e000, 0x2000, CRC(58c670e7) SHA1(db7cc24ffa3e1dec79bb2df3890680d7463c54a5) )
	ROM_LOAD( "tsl1.c1",   0x30000, 0x2000, CRC(630521f8) SHA1(204882477d5c157db6a2f702fc196792102bfbfd) )
	ROM_LOAD( "tsl1.c3",   0x32000, 0x2000, CRC(d0d7d908) SHA1(eecc9a3b8135aaed09a81a9fee0899c21ed72e7e) )
	ROM_LOAD( "tsl1.c4",   0x34000, 0x2000, CRC(dc2193c4) SHA1(11ff85c7ed65790c0570144b853ba07292e1f10d) )
	ROM_LOAD( "tsl1.c6",   0x36000, 0x2000, CRC(de417d5f) SHA1(98df166bb280d8b9c3b7a5f3e98c441ed19abc9b) )
	ROM_LOAD( "tsl1.c7",   0x38000, 0x2000, CRC(d75708c3) SHA1(1ae6b8a7fe0d43fe7428f8aeaf73e8602e05ff4a) )
	ROM_LOAD( "tsl1.c8",   0x3a000, 0x2000, CRC(69f639fd) SHA1(dfb5b8c532d3d1d9082e0523e63e5c3a32a173b7) )
	ROM_LOAD( "tsl1.c10",  0x3c000, 0x2000, CRC(5977e312) SHA1(73e7ff7478ea1698c944922ab4580d386d2a7147) )
	ROM_LOAD( "tsl1.c11",  0x3e000, 0x2000, CRC(07a6a534) SHA1(1883280c588c81875ef2250955ee97e6c9c5c4c6) )
	ROM_LOAD( "tsl1.b1",   0x40000, 0x2000, CRC(771ec128) SHA1(7d906c8432332a02a160c2624e67747eb108e71e) )
	ROM_LOAD( "tsl1.b3",   0x42000, 0x2000, CRC(e57af802) SHA1(d4cde9a6492ef750ba27270b387952a7624118ec) )
	ROM_LOAD( "tsl1.b4",   0x44000, 0x2000, CRC(7d63fe09) SHA1(944f0e66294de68f679e06ddc147f62d27d22e50) )
	ROM_LOAD( "tsl1.b6",   0x46000, 0x2000, CRC(e6c85d95) SHA1(f3d9b9fb58279df852e53f0221e087b62112df5b) )
	ROM_LOAD( "tsl1.b7",   0x48000, 0x2000, CRC(babb7e24) SHA1(aa948a57a8be1f8dc3405cfff428d629139ebd22) )
	ROM_LOAD( "tsl1.b8",   0x4a000, 0x2000, CRC(cc770802) SHA1(3830a7cb22e30e7af5a693fac3dad0f306a88c2b) )
	ROM_LOAD( "tsl1.b10",  0x4c000, 0x2000, CRC(079d0a1d) SHA1(91ee751e27b963b98774181f5037e3e88b4877df) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "tsa1.1h",   0x0e000, 0x2000, CRC(35a1dd40) SHA1(2a18b166f9ad2b6afc9e8448287228cd81d34f94) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "tsa1.2k",   0x00000, 0x2000, CRC(c0b7c8f9) SHA1(1d54da254d2524f3df49df6ad6961770852a663e) )
	ROM_LOAD( "tsa1.2l",   0x02000, 0x2000, CRC(d46f2f23) SHA1(6d3f9cf9f9d05faea86323a7752eea9467d6edc5) )
	ROM_LOAD( "tsa1.2m",   0x04000, 0x2000, CRC(04722ee4) SHA1(ab5d730330b98365fc02c38eb8545e5e1de4e93f) )
	ROM_LOAD( "tsa1.2n",   0x06000, 0x2000, CRC(266348a2) SHA1(957b541a4f3278d22153f20cdfa1ad3d31e77f4a) )
	ROM_LOAD( "tsa1.2p",   0x08000, 0x2000, CRC(33491a21) SHA1(64b2b7022297e5ee77b961b5cc751d12b711ad9b) )
	ROM_LOAD( "tsa1.2r",   0x0a000, 0x2000, CRC(669fb97a) SHA1(005c97b7f63970fce083513b485fd1f1226d90a1) )
	ROM_LOAD( "tsa1.2s",   0x0c000, 0x2000, CRC(a9167bc2) SHA1(47698943ea923c0f0c2dc60fab4c421417c1f16f) )
	ROM_LOAD( "tsa1.2t",   0x0e000, 0x2000, CRC(26bcd7ff) SHA1(7b037b7fb387420fb616db40cf0850d8b4b26a11) )
	ROM_LOAD( "tsa1.1k",   0x10000, 0x2000, CRC(60e9035d) SHA1(9ab7bbfbc7999f530f7e1921c8bdc0eefe05f9b0) )
	ROM_LOAD( "tsa1.1l",   0x12000, 0x2000, CRC(7de3bfa7) SHA1(618cb2419f3f574948cf7188de61a9eb3e6a5ff2) )
	ROM_LOAD( "tsa1.1m",   0x14000, 0x2000, CRC(77a86aef) SHA1(7f65766cd27e01fc9561ded622e3f27276f0cdb6) )
	ROM_LOAD( "tsa1.1n",   0x16000, 0x2000, CRC(4ffc26a7) SHA1(165e94458a4677defc5ca6e696520410d8e886c8) )
	ROM_LOAD( "tsa1.1p",   0x18000, 0x2000, CRC(d0c699aa) SHA1(c58319bef023c3266d42b7ae8ea8887857268330) )
	ROM_LOAD( "tsa1.1r",   0x1a000, 0x2000, CRC(753f0a5f) SHA1(24cda4f879c594840b1190e733d44bb2d59e9102) )
	ROM_LOAD( "tsa1.1s",   0x1c000, 0x2000, CRC(745f9340) SHA1(49ddae23057fbc1bc71afe4ebccbfc73a08953dc) )
	ROM_LOAD( "tsa1.1t",   0x1e000, 0x2000, CRC(0e74b7a6) SHA1(6d0c4e84fb2793aa86b3b7612aeac4a2d1394434) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "77l.12h",    0x00000, 0x0100, CRC(ef54343e) SHA1(9a85325d0814a3836b8c962c0c50b55012806b96) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( hitnmiss )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "hml3.a1",   0x08000, 0x2000, CRC(d79ae18e) SHA1(bec0b583e7667390a0f97efeb031d97f5027a827) )
	ROM_LOAD( "hml3.a3",   0x0a000, 0x2000, CRC(61baf38b) SHA1(ac92baea5f235204e06ece2c1b7d674bcad0d265) )
	ROM_LOAD( "hml3.a4",   0x0c000, 0x2000, CRC(60ca260b) SHA1(21261e30465ce0c090adad10b88c3126f47d4178) )
	ROM_LOAD( "hml3.a6",   0x0e000, 0x2000, CRC(073305d8) SHA1(2b7b8734efe57d81484174a277790ecacdf89641) )
	ROM_LOAD( "hml3.d6",   0x26000, 0x2000, CRC(79578952) SHA1(993040ab401b2ad3d7278f6a2e94a8b7162b2214) )
	ROM_LOAD( "hml3.d7",   0x28000, 0x2000, CRC(8043b78e) SHA1(67b7621abfa2a94adaa83ab166240415f137238a) )
	ROM_LOAD( "hml3.d8",   0x2a000, 0x2000, CRC(a6494e2e) SHA1(6638dd8171e6c27c0ea7d13a3f072e7d886dccd6) )
	ROM_LOAD( "hml3.d10",  0x2c000, 0x2000, CRC(0810cc84) SHA1(3b556d366a3b2e7a096a0f93a3d0c23747c27225) )
	ROM_LOAD( "hml3.d11",  0x2e000, 0x2000, CRC(9f5c3799) SHA1(3ee569e9626a399c07e5f854ba298965e41d3993) )
	ROM_LOAD( "hml3.c1",   0x30000, 0x2000, CRC(6606d5a8) SHA1(14602bc3c4a977d212cc02adca775f5355ac81d4) )
	ROM_LOAD( "hml3.c3",   0x32000, 0x2000, CRC(f6b12e48) SHA1(5974b26770ca186bc253986b156c06be5a2efb4b) )
	ROM_LOAD( "hml3.c4",   0x34000, 0x2000, CRC(e5031d44) SHA1(bed6313d31094ebbf6fbeb26672b369113be9463) )
	ROM_LOAD( "hml3.c6",   0x36000, 0x2000, CRC(1b0f2f28) SHA1(5a7cf8cc95d01eb57e0220e733eafb0de856eba2) )
	ROM_LOAD( "hml3.c7",   0x38000, 0x2000, CRC(44920233) SHA1(a85b2ad3c71d0f021637e32067d8557fb40dd206) )
	ROM_LOAD( "hml3.c8",   0x3a000, 0x2000, CRC(7db26fad) SHA1(602546d36715e8efd206eb69522a287a98e8bc64) )
	ROM_LOAD( "hml3.c10",  0x3c000, 0x2000, CRC(b8f99481) SHA1(a8af6eb103a6c0981875f4d6e8da12616155b2df) )
	ROM_LOAD( "hml3.c11",  0x3e000, 0x2000, CRC(c2a0d170) SHA1(8fa38b3a0bd19db927fead87cfac42f80e7f86a2) )
	ROM_LOAD( "hml3.b1",   0x40000, 0x2000, CRC(945cb27c) SHA1(7e62aa108a868aaad2b1558a0134fb4c2426e0b5) )
	ROM_LOAD( "hml3.b3",   0x42000, 0x2000, CRC(3f022689) SHA1(476874eae92b37ccff6cf64ac74b59a4ad9a4472) )
	ROM_LOAD( "hml3.b4",   0x44000, 0x2000, CRC(d63fd250) SHA1(5949a513cdd65d65cd923a788651886f8ab583cc) )
	ROM_LOAD( "hml3.b6",   0x46000, 0x2000, CRC(afc89eed) SHA1(17053d6f304a7ef7cb01713dbcea913125236185) )
	ROM_LOAD( "hml3.b7",   0x48000, 0x2000, CRC(f3a12a58) SHA1(f278a6597fca15746cc50f4ced5fa76f530c2a16) )
	ROM_LOAD( "hml3.b8",   0x4a000, 0x2000, CRC(e0a5a6aa) SHA1(b012a1e23fd0acf9972714ed8aea0cedbb079a31) )
	ROM_LOAD( "hml3.b10",  0x4c000, 0x2000, CRC(de65dfdc) SHA1(c1105ff41596ee5f4c79143552eab87fcbe93d1e) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "hma3.1h",  0x0e000, 0x2000, CRC(f718da36) SHA1(6c878725e679e0c553494c621bee059fe8b67ae8) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "hm2.2k",   0x00000, 0x2000, CRC(d3583b62) SHA1(99be3a858ab6b1c950ef684202adec4f5e60a361) )
	ROM_LOAD( "hm2.2l",   0x02000, 0x2000, CRC(c059d51e) SHA1(ddf437cdff6168e76c6a65078e0a2e2862805ca7) )
	ROM_LOAD( "hma.2m",   0x04000, 0x2000, CRC(09bb8495) SHA1(ea817cbbd89aa18d81f6025a856965d466efadff) )
	ROM_LOAD( "hma.2n",   0x06000, 0x2000, CRC(e3d290df) SHA1(5e711297738c702e29052a721cc814db4fc78d82) )
	ROM_LOAD( "hma.2p",   0x08000, 0x2000, CRC(f93d1ac0) SHA1(7c39f3891315bbaa01f56ce853b12077ed6e4441) )
	ROM_LOAD( "hma.2r",   0x0a000, 0x2000, CRC(0f3a090e) SHA1(d98e7c3e56973314b3498f515efb79279a836634) )
	ROM_LOAD( "hma.2s",   0x0c000, 0x2000, CRC(c5d35f84) SHA1(a305fc1196f76f9c8cd84c66ab8a05421e5856fa) )
	ROM_LOAD( "hma.2t",   0x0e000, 0x2000, CRC(9aa71457) SHA1(fa2bdebfee025022afdb10c483ccbf0d297b48fb) )
	ROM_LOAD( "xba.1n",   0x16000, 0x2000, CRC(2e855698) SHA1(fa4c3ec03fdd1c569c0ca2418899ffa81b5259ec) )
	ROM_LOAD( "hma.1p",   0x18000, 0x2000, CRC(021d89dd) SHA1(97501ab4c1b5fe55366e9d4831755ea43d759613) )
	ROM_LOAD( "hma.1r",   0x1a000, 0x2000, CRC(e8bb33bc) SHA1(246cbd125be5bc17b2693231bab1eafc0ed686bb) )
	ROM_LOAD( "hma.1s",   0x1c000, 0x2000, CRC(65f1aa6e) SHA1(5385d76fbcf551e26629544ddee7ca6a4b447a4a) )
	ROM_LOAD( "hma.1t",   0x1e000, 0x2000, CRC(eb35dfcc) SHA1(97ef32874c1f3cd3370b5784642c7c7bd134f54d) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( hitnmiss2 )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "hml2.a1",   0x08000, 0x2000, CRC(322f7e83) SHA1(e80e51c943402d0b644b1788e6a278130610d0e6) )
	ROM_LOAD( "hml2.a3",   0x0a000, 0x2000, CRC(0e12a721) SHA1(22ee013d68c1afc218672cbb40c99fc9de607328) )
	ROM_LOAD( "hml2.a4",   0x0c000, 0x2000, CRC(6cec8ad2) SHA1(7f755a3a79d913a549bda8a4f9d156ff152b7d7b) )
	ROM_LOAD( "hml2.a6",   0x0e000, 0x2000, CRC(008803ec) SHA1(3437103dc8698f9054363e188a1b2390b5ae88c4) )
	ROM_LOAD( "hml2.d4",   0x24000, 0x2000, CRC(62790789) SHA1(5ca02688e78ca79ae6b2e5c52df80788feb0e476) )
	ROM_LOAD( "hml2.d6",   0x26000, 0x2000, CRC(02d2d07e) SHA1(efedf61eb71a98b2f717bf95633fb85f7b1b7a0a) )
	ROM_LOAD( "hml2.d7",   0x28000, 0x2000, CRC(0f795f7a) SHA1(2f18e065e0b3567fa8dd8a7fe8d5cd37014e7e0e) )
	ROM_LOAD( "hml2.d8",   0x2a000, 0x2000, CRC(fe40c51d) SHA1(585c09b0c3f15ab0f37a8f32f6798280c57799f0) )
	ROM_LOAD( "hml2.d10",  0x2c000, 0x2000, CRC(9362c90c) SHA1(5fdd1afe28d310abb1b00e23df44ce2c2b584012) )
	ROM_LOAD( "hml2.d11",  0x2e000, 0x2000, CRC(0f77322f) SHA1(c02e8ebf2e1f53704069be747390a3776de2c67d) )
	ROM_LOAD( "hml2.c1",   0x30000, 0x2000, CRC(8e5803ff) SHA1(422b62227fc1e2715b5075c922ea8b948ee37a12) )
	ROM_LOAD( "hml2.c3",   0x32000, 0x2000, CRC(a7474441) SHA1(e7ba25785b9a276f875c16cd229a8b5e5e5cdd44) )
	ROM_LOAD( "hml2.c4",   0x34000, 0x2000, CRC(c74b9610) SHA1(415488b2bacdf3b5ccecfc75abe275be665b467d) )
	ROM_LOAD( "hml2.c6",   0x36000, 0x2000, CRC(aca63300) SHA1(d53f7305383c83c08f8777bb5fdcaf3974c2b19d) )
	ROM_LOAD( "hml2.c7",   0x38000, 0x2000, CRC(1990305e) SHA1(225f75165e0de26b07bdfdb1121f8545f56e621f) )
	ROM_LOAD( "hml2.c8",   0x3a000, 0x2000, CRC(bf08cf05) SHA1(6f8f53b0aa39d3edc6e6e0c3403d88be5a358223) )
	ROM_LOAD( "hml2.c10",  0x3c000, 0x2000, CRC(971323ca) SHA1(79b43ee142717533ffe06a95d80de238177d7d87) )
	ROM_LOAD( "hml2.c11",  0x3e000, 0x2000, CRC(dd172feb) SHA1(432a7383c7ad5a60dd53e12c1ca23d603302f8d4) )
	ROM_LOAD( "hml2.b1",   0x40000, 0x2000, CRC(af1fce57) SHA1(10336514d4dc3166f22f96e0083a9828b41aff83) )
	ROM_LOAD( "hml2.b3",   0x42000, 0x2000, CRC(0d16ef47) SHA1(0403cd40b421ae142e2e0e6aff4d720d6dfd9768) )
	ROM_LOAD( "hml2.b4",   0x44000, 0x2000, CRC(d5a8ff68) SHA1(daba4344eb29c9854773d03e57087e460ea8e1b6) )
	ROM_LOAD( "hml2.b6",   0x46000, 0x2000, CRC(13f439b1) SHA1(d7b372bc9800bbfaee9b080ee6727491295dd0df) )
	ROM_LOAD( "hml2.b7",   0x48000, 0x2000, CRC(9088c16d) SHA1(142d5b58ee95cf097bfe627ba8737494df9a231e) )
	ROM_LOAD( "hml2.b8",   0x4a000, 0x2000, CRC(9c2db94a) SHA1(aa90181c0cc3e130f872ff5beb2be340e7851e1a) )
	ROM_LOAD( "hml2.b10",  0x4c000, 0x2000, CRC(f01bd7d4) SHA1(169139c89582852b6141fd37e75486753674c557) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "hma2.1h",  0x0e000, 0x2000, CRC(9be48f45) SHA1(360138e3996828509b4bd1b3efccd61f05d215f0) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "hm2.2k",   0x00000, 0x2000, CRC(d3583b62) SHA1(99be3a858ab6b1c950ef684202adec4f5e60a361) )
	ROM_LOAD( "hm2.2l",   0x02000, 0x2000, CRC(c059d51e) SHA1(ddf437cdff6168e76c6a65078e0a2e2862805ca7) )
	ROM_LOAD( "hma.2m",   0x04000, 0x2000, CRC(09bb8495) SHA1(ea817cbbd89aa18d81f6025a856965d466efadff) )
	ROM_LOAD( "hma.2n",   0x06000, 0x2000, CRC(e3d290df) SHA1(5e711297738c702e29052a721cc814db4fc78d82) )
	ROM_LOAD( "hma.2p",   0x08000, 0x2000, CRC(f93d1ac0) SHA1(7c39f3891315bbaa01f56ce853b12077ed6e4441) )
	ROM_LOAD( "hma.2r",   0x0a000, 0x2000, CRC(0f3a090e) SHA1(d98e7c3e56973314b3498f515efb79279a836634) )
	ROM_LOAD( "hma.2s",   0x0c000, 0x2000, CRC(c5d35f84) SHA1(a305fc1196f76f9c8cd84c66ab8a05421e5856fa) )
	ROM_LOAD( "hma.2t",   0x0e000, 0x2000, CRC(9aa71457) SHA1(fa2bdebfee025022afdb10c483ccbf0d297b48fb) )
	ROM_LOAD( "xba.1n",   0x16000, 0x2000, CRC(2e855698) SHA1(fa4c3ec03fdd1c569c0ca2418899ffa81b5259ec) )
	ROM_LOAD( "hma.1p",   0x18000, 0x2000, CRC(021d89dd) SHA1(97501ab4c1b5fe55366e9d4831755ea43d759613) )
	ROM_LOAD( "hma.1r",   0x1a000, 0x2000, CRC(e8bb33bc) SHA1(246cbd125be5bc17b2693231bab1eafc0ed686bb) )
	ROM_LOAD( "hma.1s",   0x1c000, 0x2000, CRC(65f1aa6e) SHA1(5385d76fbcf551e26629544ddee7ca6a4b447a4a) )
	ROM_LOAD( "hma.1t",   0x1e000, 0x2000, CRC(eb35dfcc) SHA1(97ef32874c1f3cd3370b5784642c7c7bd134f54d) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( whodunit ) /* Version 9 */
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "wdl-9_1-a_2764.bin", 0x08000, 0x2000, CRC(903e45f6) SHA1(3c5d3f0ec851331febf7da493bfa3b33ac5aaff3) )
	ROM_LOAD( "wdl8.3a",   0x0a000, 0x2000, CRC(5d1530f8) SHA1(258d19c14a757497e0ce95a3ab8332168d97e67c) ) // WDL-9_3-A_2764.bin
	ROM_LOAD( "wdl-9_4-a_2764.bin", 0x0c000, 0x2000, CRC(96c6b81f) SHA1(c4775b7cdbd2c977131b0df99aef75834a68559f) )
	ROM_LOAD( "wdl-9_6-a_2764.bin", 0x0e000, 0x2000, CRC(c9a864ec) SHA1(2f67b8b711101818888bb9624be8a33d98ddc811) )
	ROM_LOAD( "wdl-9_4-e_2764.bin", 0x14000, 0x2000, CRC(246c836f) SHA1(1c39b61dbbde5df2a79a9c96ed2a85eaf870b46c) )
	ROM_LOAD( "wdl-9_6-e_2764.bin", 0x16000, 0x2000, CRC(65d551c0) SHA1(7ad3c2567ad7f64b8c3b9fade01296bad1ffff89) )
	ROM_LOAD( "wdl-9_7-e_2764.bin", 0x18000, 0x2000, CRC(aee7a237) SHA1(e12a5cc825944c95461b436041631bc64ae7f0c3) )
	ROM_LOAD( "wdl-9_8-e_2764.bin", 0x1a000, 0x2000, CRC(a1c1e995) SHA1(7fdd54102ba3d1a4e1612d1e63ac435317184c03) )
	ROM_LOAD( "wdl8.10e",  0x1c000, 0x2000, CRC(f24adde5) SHA1(e4b22da75341a70c75fa245e26e965f1f7cbf3a1) ) // WDL-9_10-E_2764.bin
	ROM_LOAD( "wdl8.11e",  0x1e000, 0x2000, CRC(ad6fe69e) SHA1(039baebb3a232cbd2c497cfe8aba234469e16ada) ) // WDL-9_11-E_2764.bin
	ROM_LOAD( "wdl6.1d",   0x20000, 0x2000, CRC(3572fb71) SHA1(dcc5b04c8a4c0db62eeffe8524f67574142f1a19) ) // WDL-9_1-D_2764.bin
	ROM_LOAD( "wdl6.3d",   0x22000, 0x2000, CRC(158074f4) SHA1(d4520017973a4476ba9ed7b5ed7cbfc3f5302f3a) ) // WDL-9_3-D_2764.bin
	ROM_LOAD( "wdl8.4d",   0x24000, 0x2000, CRC(601d8bd0) SHA1(118e5d7358034c98a1e4e87b1e523e574c8264bb) ) // WDL-9_4-D_2764.bin
	ROM_LOAD( "wdl8.6d",   0x26000, 0x2000, CRC(b72e8f63) SHA1(bdb5b379d5c14123e59cd186499703ca2096027c) ) // WDL-9_6-D_2764.bin
	ROM_LOAD( "wdl6.7d",   0x28000, 0x2000, CRC(e3f55a4b) SHA1(9d3e4fc68f9b8c7228ef407f039849fdcdb1d9ef) ) // WDL-9_7-D_2764.bin
	ROM_LOAD( "wdl6.8d",   0x2a000, 0x2000, CRC(932689c8) SHA1(9b65db2c4ce544331290f3dd57fde3b6aab905ff) ) // WDL-9_8-D_2764.bin
	ROM_LOAD( "wdl6.10d",  0x2c000, 0x2000, CRC(0c4348f2) SHA1(19a3919dccf198c2189e310545aa15b319990579) ) // WDL-9_10-D_2764.bin
	ROM_LOAD( "wdl8.11d",  0x2e000, 0x2000, CRC(92391ffe) SHA1(1f4bb288ec80af3eb452ca8fe67cba862e23977b) ) // WDL-9_11-D_2764.bin
	ROM_LOAD( "wdl8.1c",   0x30000, 0x2000, CRC(21c62c90) SHA1(98dbf416e9d88383bf3e0e17a71861671046aa20) ) // WDL-9_1-C_2764.bin
	ROM_LOAD( "wdl-9_3-c_2764.bin", 0x32000, 0x2000, CRC(b221188e) SHA1(0f488e7a46489e38da322769189c2f992bb3e1e7) )
	ROM_LOAD( "wdl6.4c",   0x34000, 0x2000, CRC(7b58dfac) SHA1(b52566a9bfcd30510accc7e32c3535b99bf3372c) ) // WDL-9_4-C_2764.bin
	ROM_LOAD( "wdl-9_6-c_2764.bin", 0x36000, 0x2000, CRC(abe0ba57) SHA1(9dfcdf8f7b8f09d6d2e90e1bb1fda9064215fe14) )
	ROM_LOAD( "wdl6.7c",   0x38000, 0x2000, CRC(3cb3faae) SHA1(de719bc4806aa54ff73a723e3dfffed8bb394da0) ) // WDL-9_7-C_2764.bin
	ROM_LOAD( "wdl6.8c",   0x3a000, 0x2000, CRC(51e95d91) SHA1(091dca2360456988da9ed39e757918ec58118258) ) // WDL-9_8-C_2764.bin
	ROM_LOAD( "wdl6.10c",  0x3c000, 0x2000, CRC(eab12084) SHA1(aa75dfdeece07baa6cf72cecfc006af958768acf) ) // WDL-9_10-C_2764.bin
	ROM_LOAD( "wdl6.11c",  0x3e000, 0x2000, CRC(fe2c532f) SHA1(199334fdc4c92ecacf4d4337bbe7419a53b78067) ) // WDL-9_11-C_2764.bin
	ROM_LOAD( "wdl6.1b",   0x40000, 0x2000, CRC(cdaa5ca0) SHA1(1232e54cd61293f1cb7a8c628bb811bfd527ba93) ) // WDL-9_1-B_2764.bin
	ROM_LOAD( "wdl6.3b",   0x42000, 0x2000, CRC(9bbc8161) SHA1(a4849ff8c957904a87f53edf79961a035e9b7c6b) ) // WDL-9_3-B_2764.bin
	ROM_LOAD( "wdl8.4b",   0x44000, 0x2000, CRC(eb7dc583) SHA1(fe61e1eb16be246493db40f90090317d60a21bbf) ) // WDL-9_4-B_2764.bin
	ROM_LOAD( "wdl8.6b",   0x46000, 0x2000, CRC(91c049a5) SHA1(8c6ae1e80925526902f80a2c549c6739353cdb86) ) // WDL-9_6-B_2764.bin
	ROM_LOAD( "wdl6.7b",   0x48000, 0x2000, CRC(fe0a2d00) SHA1(a8649a95567f6c1f6513461a19250455b8cfe3ed) ) // WDL-9_7-B_2764.bin
	ROM_LOAD( "wdl8.8b",   0x4a000, 0x2000, CRC(33792758) SHA1(408da288288f54f7446b083b14dc74d43ef4ab9f) ) // WDL-9_8-B_2764.bin
	ROM_LOAD( "wdl-9_10-b_2764.bin",0x4c000, 0x2000, CRC(c5ab5805) SHA1(fd7c47e50eb4005b81309a73afae2e04a823d00b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "wda-9_h-1_2764.bin", 0x0e000, 0x2000, CRC(dc4b36f0) SHA1(1ddd47dbd7f3e360aae830b67a13dd6a1d7a6497) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "wda6.k2",   0x00000, 0x2000, CRC(d4951375) SHA1(88616a7cb587748b366ece6126185a85e7375659) ) // WDL-9_2-K_2764.bin
	ROM_LOAD( "wda6.l2",   0x02000, 0x2000, CRC(be8dcf07) SHA1(9a6e9b256da07be50feb81b27e53d86b3f016f4e) ) // WDL-9_2-L_2764.bin
	ROM_LOAD( "wda6.m2",   0x04000, 0x2000, CRC(fb389e2d) SHA1(8ee1be233429d6b7cbb56a13586e2db49dffaca1) ) // WDL-9_2-M_2764.bin
	ROM_LOAD( "wda6.n2",   0x06000, 0x2000, CRC(3849bc78) SHA1(2b5bffec6919c3f4dfd3618f715445b02905cc1d) ) // WDL-9_2-N_2764.bin
	ROM_LOAD( "wda6.p2",   0x08000, 0x2000, CRC(d0dcea80) SHA1(ac3305eab146f1741d38678a6f473474bef05ade) ) // WDL-9_2-P_2764.bin
	ROM_LOAD( "wda6.r2",   0x0a000, 0x2000, CRC(748b0930) SHA1(f7e06ca6bef86d2bc8fa4569bcfc8d2ec199e4bd) ) // WDL-9_2-R_2764.bin
	ROM_LOAD( "wda6.s2",   0x0c000, 0x2000, CRC(23d5c5a9) SHA1(ab8997556b6a9c4a011c367a2035aeba3c752be1) ) // WDL-9_2-S_2764.bin
	ROM_LOAD( "wda6.t2",   0x0e000, 0x2000, CRC(a807536d) SHA1(154564c189c7e6f755acda95178db503991ecbaa) ) // WDL-9_2-T_2764.bin
	ROM_LOAD( "wda8.l1",   0x12000, 0x2000, CRC(27b856bd) SHA1(f66f6f898d2a7f044b7d331290a7bf32715b5587) ) // WDL-9_1-L_2764.bin
	ROM_LOAD( "wda8.m1",   0x14000, 0x2000, CRC(8e15c601) SHA1(924b10523cf8ff802c0907dae96cbc9bae9fe4b0) ) // WDL-9_1-M_2764.bin
	ROM_LOAD( "xba1.1n",   0x16000, 0x2000, CRC(2e855698) SHA1(fa4c3ec03fdd1c569c0ca2418899ffa81b5259ec) ) // WDL-9_1-N_2764.bin
	ROM_LOAD( "wda6.p1",   0x18000, 0x2000, CRC(3ffaaa22) SHA1(a0848c0f4d799c3b6e9fe8e8f89e7e36754174f6) ) // WDL-9_1-P_2764.bin
	ROM_LOAD( "wda6.r1",   0x1a000, 0x2000, CRC(0579a3b8) SHA1(554bced664c12547a766ee6df1278b967714b5ae) ) // WDL-9_1-R_2764.bin
	ROM_LOAD( "wda6.s1",   0x1c000, 0x2000, CRC(f55c3c6e) SHA1(54878fe1106986e9aa26c6141921429119703990) ) // WDL-9_1-S_2764.bin
	ROM_LOAD( "wda6.t1",   0x1e000, 0x2000, CRC(38363b52) SHA1(4b1c3225a32882e35e5af742f1c6b1e665722fd4) ) // WDL-9_1-T_2764.bin

	ROM_REGION( 0x01100, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) ) // XBL-12H_MMI_6301.bin
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) ) // XBL-9H_MMI_6301.bin
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) ) // XBL-2H_MMI_6301.bin
	ROM_LOAD( "xbl-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) ) // XBL-4K_MMI_6301.bin
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) ) // XBL-5K_MMI_6301.bin
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) ) // XBL-6K_MMI_6301.bin
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) ) // XBL-7K_MMI_6301.bin
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) ) // XBL-8K_MMI_6301.bin
	ROM_LOAD( "11b_xicor_2804a.bin", 0x0900, 0x0200, CRC(ec4a70e2) SHA1(2dc271571d320660fb6484dbc1bd6daa73be05ae) ) // Unknown Use - Dumped from Version 9 PCB
ROM_END


ROM_START( whodunit8 ) /* Version 8 */
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "wdl8.1a",   0x08000, 0x2000, CRC(50658904) SHA1(5d1b44a0e20c4ec599c339235f42ca881f8a5bfc) )
	ROM_LOAD( "wdl8.3a",   0x0a000, 0x2000, CRC(5d1530f8) SHA1(258d19c14a757497e0ce95a3ab8332168d97e67c) )
	ROM_LOAD( "wdl8.4a",   0x0c000, 0x2000, CRC(0323d6b8) SHA1(b85ca3f6ec6599fbdf5db3901c23cf6a3feb832e) )
	ROM_LOAD( "wdl8.6a",   0x0e000, 0x2000, CRC(771b3fb1) SHA1(d296b397387a813e831bd187657b8dc1150e78c6) )
	ROM_LOAD( "wdl8.4e",   0x14000, 0x2000, CRC(33e44369) SHA1(9451b439b25d0d303ebbd72d63637f87ddd7c4fa) )
	ROM_LOAD( "wdl8.6e",   0x16000, 0x2000, CRC(64b1d850) SHA1(d49cf787e885cc10030b4889bb8890bef92d3b01) )
	ROM_LOAD( "wdl8.7e",   0x18000, 0x2000, CRC(aa54cf90) SHA1(0c303f71b871e3f44fb709d1f3cf50f76790fb21) )
	ROM_LOAD( "wdl8.8e",   0x1a000, 0x2000, CRC(cbd61200) SHA1(f8fbf291981ee63b4e32491aebbc37c5e3e55bbb) )
	ROM_LOAD( "wdl8.10e",  0x1c000, 0x2000, CRC(f24adde5) SHA1(e4b22da75341a70c75fa245e26e965f1f7cbf3a1) )
	ROM_LOAD( "wdl8.11e",  0x1e000, 0x2000, CRC(ad6fe69e) SHA1(039baebb3a232cbd2c497cfe8aba234469e16ada) )
	ROM_LOAD( "wdl6.1d",   0x20000, 0x2000, CRC(3572fb71) SHA1(dcc5b04c8a4c0db62eeffe8524f67574142f1a19) )
	ROM_LOAD( "wdl6.3d",   0x22000, 0x2000, CRC(158074f4) SHA1(d4520017973a4476ba9ed7b5ed7cbfc3f5302f3a) )
	ROM_LOAD( "wdl8.4d",   0x24000, 0x2000, CRC(601d8bd0) SHA1(118e5d7358034c98a1e4e87b1e523e574c8264bb) )
	ROM_LOAD( "wdl8.6d",   0x26000, 0x2000, CRC(b72e8f63) SHA1(bdb5b379d5c14123e59cd186499703ca2096027c) )
	ROM_LOAD( "wdl6.7d",   0x28000, 0x2000, CRC(e3f55a4b) SHA1(9d3e4fc68f9b8c7228ef407f039849fdcdb1d9ef) )
	ROM_LOAD( "wdl6.8d",   0x2a000, 0x2000, CRC(932689c8) SHA1(9b65db2c4ce544331290f3dd57fde3b6aab905ff) )
	ROM_LOAD( "wdl6.10d",  0x2c000, 0x2000, CRC(0c4348f2) SHA1(19a3919dccf198c2189e310545aa15b319990579) )
	ROM_LOAD( "wdl8.11d",  0x2e000, 0x2000, CRC(92391ffe) SHA1(1f4bb288ec80af3eb452ca8fe67cba862e23977b) )
	ROM_LOAD( "wdl8.1c",   0x30000, 0x2000, CRC(21c62c90) SHA1(98dbf416e9d88383bf3e0e17a71861671046aa20) )
	ROM_LOAD( "wdl8.3c",   0x32000, 0x2000, CRC(5a8123be) SHA1(c548384998a81950a4ee0f56e07698edfbb5bc88) )
	ROM_LOAD( "wdl6.4c",   0x34000, 0x2000, CRC(7b58dfac) SHA1(b52566a9bfcd30510accc7e32c3535b99bf3372c) )
	ROM_LOAD( "wdl6.6c",   0x36000, 0x2000, CRC(9be0b9a9) SHA1(62404abee217f332fce25941919aa87b85733182) )
	ROM_LOAD( "wdl6.7c",   0x38000, 0x2000, CRC(3cb3faae) SHA1(de719bc4806aa54ff73a723e3dfffed8bb394da0) )
	ROM_LOAD( "wdl6.8c",   0x3a000, 0x2000, CRC(51e95d91) SHA1(091dca2360456988da9ed39e757918ec58118258) )
	ROM_LOAD( "wdl6.10c",  0x3c000, 0x2000, CRC(eab12084) SHA1(aa75dfdeece07baa6cf72cecfc006af958768acf) )
	ROM_LOAD( "wdl6.11c",  0x3e000, 0x2000, CRC(fe2c532f) SHA1(199334fdc4c92ecacf4d4337bbe7419a53b78067) )
	ROM_LOAD( "wdl6.1b",   0x40000, 0x2000, CRC(cdaa5ca0) SHA1(1232e54cd61293f1cb7a8c628bb811bfd527ba93) )
	ROM_LOAD( "wdl6.3b",   0x42000, 0x2000, CRC(9bbc8161) SHA1(a4849ff8c957904a87f53edf79961a035e9b7c6b) )
	ROM_LOAD( "wdl8.4b",   0x44000, 0x2000, CRC(eb7dc583) SHA1(fe61e1eb16be246493db40f90090317d60a21bbf) )
	ROM_LOAD( "wdl8.6b",   0x46000, 0x2000, CRC(91c049a5) SHA1(8c6ae1e80925526902f80a2c549c6739353cdb86) )
	ROM_LOAD( "wdl6.7b",   0x48000, 0x2000, CRC(fe0a2d00) SHA1(a8649a95567f6c1f6513461a19250455b8cfe3ed) )
	ROM_LOAD( "wdl8.8b",   0x4a000, 0x2000, CRC(33792758) SHA1(408da288288f54f7446b083b14dc74d43ef4ab9f) )
	ROM_LOAD( "wdl6.10b",  0x4c000, 0x2000, CRC(2f48cfdb) SHA1(b546da26b7bdc52c454ff32e4503ef5e45e4b360) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "wda8.h1",  0x0e000, 0x2000, CRC(0090e5a7) SHA1(c97e4c83d507d1375320aa9cae07b9fa1ee442c8) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "wda6.k2",   0x00000, 0x2000, CRC(d4951375) SHA1(88616a7cb587748b366ece6126185a85e7375659) )
	ROM_LOAD( "wda6.l2",   0x02000, 0x2000, CRC(be8dcf07) SHA1(9a6e9b256da07be50feb81b27e53d86b3f016f4e) )
	ROM_LOAD( "wda6.m2",   0x04000, 0x2000, CRC(fb389e2d) SHA1(8ee1be233429d6b7cbb56a13586e2db49dffaca1) )
	ROM_LOAD( "wda6.n2",   0x06000, 0x2000, CRC(3849bc78) SHA1(2b5bffec6919c3f4dfd3618f715445b02905cc1d) )
	ROM_LOAD( "wda6.p2",   0x08000, 0x2000, CRC(d0dcea80) SHA1(ac3305eab146f1741d38678a6f473474bef05ade) )
	ROM_LOAD( "wda6.r2",   0x0a000, 0x2000, CRC(748b0930) SHA1(f7e06ca6bef86d2bc8fa4569bcfc8d2ec199e4bd) )
	ROM_LOAD( "wda6.s2",   0x0c000, 0x2000, CRC(23d5c5a9) SHA1(ab8997556b6a9c4a011c367a2035aeba3c752be1) )
	ROM_LOAD( "wda6.t2",   0x0e000, 0x2000, CRC(a807536d) SHA1(154564c189c7e6f755acda95178db503991ecbaa) )
	ROM_LOAD( "wda8.l1",   0x12000, 0x2000, CRC(27b856bd) SHA1(f66f6f898d2a7f044b7d331290a7bf32715b5587) )
	ROM_LOAD( "wda8.m1",   0x14000, 0x2000, CRC(8e15c601) SHA1(924b10523cf8ff802c0907dae96cbc9bae9fe4b0) )
	ROM_LOAD( "xba1.1n",   0x16000, 0x2000, CRC(2e855698) SHA1(fa4c3ec03fdd1c569c0ca2418899ffa81b5259ec) )
	ROM_LOAD( "wda6.p1",   0x18000, 0x2000, CRC(3ffaaa22) SHA1(a0848c0f4d799c3b6e9fe8e8f89e7e36754174f6) )
	ROM_LOAD( "wda6.r1",   0x1a000, 0x2000, CRC(0579a3b8) SHA1(554bced664c12547a766ee6df1278b967714b5ae) )
	ROM_LOAD( "wda6.s1",   0x1c000, 0x2000, CRC(f55c3c6e) SHA1(54878fe1106986e9aa26c6141921429119703990) )
	ROM_LOAD( "wda6.t1",   0x1e000, 0x2000, CRC(38363b52) SHA1(4b1c3225a32882e35e5af742f1c6b1e665722fd4) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( showdown )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "showda1.bin",   0x08000, 0x2000, CRC(e4031507) SHA1(3d5183cd049be843fd38e3d1bd2d0a305df723bd) )
	ROM_LOAD( "showd3a.bin",   0x0a000, 0x2000, CRC(e7de171e) SHA1(881a2b596949de3b4bb1263e2aa08faeb3051a6e) )
	ROM_LOAD( "showd4a.bin",   0x0c000, 0x2000, CRC(5c8683c9) SHA1(81d0880fcbd3c1662ea4dd1662d6987adbdb4f71) )
	ROM_LOAD( "showd6a.bin",   0x0e000, 0x2000, CRC(4a408379) SHA1(1ec83b7416f948bc31bafa8ddaa87c23490bce16) )
	ROM_LOAD( "showd11e.bin",  0x1e000, 0x2000, CRC(1c6b34e5) SHA1(ae5ddd80d5fdc89274f44a30c4ec1aa325b26cc7) )
	ROM_LOAD( "showd1d.bin",   0x20000, 0x2000, CRC(db4c8cf6) SHA1(11fb37afb87b926f94f23abf90fb537a3a867aec) )
	ROM_LOAD( "showd3d.bin",   0x22000, 0x2000, CRC(24242867) SHA1(aa109231cad5fcb9e24578b567ff0fe50a72be44) )
	ROM_LOAD( "showd4d.bin",   0x24000, 0x2000, CRC(36f247e9) SHA1(749f5ea3307bfc02bced9535ebd733bb1297a0ae) )
	ROM_LOAD( "showd6d.bin",   0x26000, 0x2000, CRC(c9b14d8d) SHA1(48e66f5a4dc63c3948e32aeb0a151c8f9d2082b2) )
	ROM_LOAD( "showd7d.bin",   0x28000, 0x2000, CRC(fd054cd2) SHA1(a7fcedd30c088d2cb9fe013eaa1214b48f3569ab) )
	ROM_LOAD( "showd8d.bin",   0x2a000, 0x2000, CRC(8bf32822) SHA1(91a1ab0bc6aac3dfe7b7e60e3d35e7d4909aa09d) )
	ROM_LOAD( "showd10d.bin",  0x2c000, 0x2000, CRC(a2051da2) SHA1(f879459a90a00c9dc4ba0b1e8895bc1352286a8a) )
	ROM_LOAD( "showd11d.bin",  0x2e000, 0x2000, CRC(0748f345) SHA1(c435cd0769aa44162b3f7aa1bc230cadf572ca73) )
	ROM_LOAD( "showd1c.bin",   0x30000, 0x2000, CRC(c016cf73) SHA1(30221e5f878354933b8caf8c644f2c6f1e5dcd30) )
	ROM_LOAD( "showd3c.bin",   0x32000, 0x2000, CRC(652503ee) SHA1(90c76bb5d59ce0626d1d7f3feaea05ef984f9551) )
	ROM_LOAD( "showd4c.bin",   0x34000, 0x2000, CRC(b4dab193) SHA1(e028d2c865e7607f43bb7b4f2afe75082618a47b) )
	ROM_LOAD( "showd6c.bin",   0x36000, 0x2000, CRC(a1e6a2b3) SHA1(b75a0355e1245a8ca0dc438c66a96c064b7ab40a) )
	ROM_LOAD( "showd7c.bin",   0x38000, 0x2000, CRC(bc1bea93) SHA1(77fcfef6c509186af394f0ad67717c11ca447fb7) )
	ROM_LOAD( "showd8c.bin",   0x3a000, 0x2000, CRC(337dd7fa) SHA1(4df916968f5a2e12cdc8bab585f58b6f6a9d2f4c) )
	ROM_LOAD( "showd10c.bin",  0x3c000, 0x2000, CRC(3ad32d71) SHA1(1a032eb136c56b97305e64572730a40c40d9c52b) )
	ROM_LOAD( "showd11c.bin",  0x3e000, 0x2000, CRC(5fe91932) SHA1(f4f880c55e72159a6754f7c939b4dbb16522e3ad) )
	ROM_LOAD( "showd1b.bin",   0x40000, 0x2000, CRC(54ff987e) SHA1(ea50a6e2b6c409403cec035f96f4672814e153e2) )
	ROM_LOAD( "showd3b.bin",   0x42000, 0x2000, CRC(e302e915) SHA1(cb2413c24503fd2363f6e717e8de558f771427a4) )
	ROM_LOAD( "showd4b.bin",   0x44000, 0x2000, CRC(1b981516) SHA1(16c417c9c1918a00dee976f3513925f8f28e6f41) )
	ROM_LOAD( "showd6b.bin",   0x46000, 0x2000, CRC(4ee00996) SHA1(7201aef40f6ea3b73d0c009117c174f19d97b98e) )
	ROM_LOAD( "showd7b.bin",   0x48000, 0x2000, CRC(018b7c00) SHA1(04879f476687e087d21bd8d5f439f3e45d39d142) )
	ROM_LOAD( "showd8b.bin",   0x4a000, 0x2000, CRC(024fe6ee) SHA1(4287091e65c58aec75c54e320c534d41def951f9) )
	ROM_LOAD( "showd10b.bin",  0x4c000, 0x2000, CRC(0b318dfe) SHA1(feb65530ea3aea6b0786875dc48d96e07d579636) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "showd1h.bin",  0x0e000, 0x2000, CRC(6a10ff47) SHA1(ee57de74ab9a5cfe5726212a9b905e91e6461225) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "showd2k.bin",   0x00000, 0x2000, CRC(67a86f7f) SHA1(a4f70aab90acd2502e8f3f39efdafcd71b1a22b4) )
	ROM_LOAD( "showd2l.bin",   0x02000, 0x2000, CRC(0bb8874b) SHA1(8b643dbd5412a713b3e2831dd1ba2b7d1f613ac2) )
	ROM_LOAD( "showd2m.bin",   0x04000, 0x2000, CRC(8b77eac8) SHA1(d70038cd6655e71c6488c555ecb1d1a424d00d49) )
	ROM_LOAD( "showd2n.bin",   0x06000, 0x2000, CRC(78e6eed6) SHA1(d9218745fc497a67373b0f6fd82caeef33bb4b3e) )
	ROM_LOAD( "showd2p.bin",   0x08000, 0x2000, CRC(03a13435) SHA1(1965d26a2f294e883cc6a13bc89bf0c28e30d28e) )
	ROM_LOAD( "showd2r.bin",   0x0a000, 0x2000, CRC(1b6b7eac) SHA1(b30965203807d8b863e8b2863a222d2e614eee2d) )
	ROM_LOAD( "showd2s.bin",   0x0c000, 0x2000, CRC(b88aeb82) SHA1(62d87430c7aec1aec892a15e45fb257206d8cf94) )
	ROM_LOAD( "showd2t.bin",   0x0e000, 0x2000, CRC(5c801f4d) SHA1(e6dbb2d2815b9d848c0580cff47d1f0d10c1906d) )
	ROM_LOAD( "showd1k.bin",   0x10000, 0x2000, CRC(4e1f4f15) SHA1(cf075e09eaeab1e630b5b2e28c806d4c34eece48) )
	ROM_LOAD( "showd1l.bin",   0x12000, 0x2000, CRC(6779a745) SHA1(de7d8e39f053eaa45238844e477732295d9af494) )
	ROM_LOAD( "showd1m.bin",   0x14000, 0x2000, CRC(9cebd8ea) SHA1(ff8121c16e8fe93a59c49c20459287f6b002bbbc) )
	ROM_LOAD( "showd1n.bin",   0x16000, 0x2000, CRC(689d8a3f) SHA1(aa592b9edcac8d264c9c89871283dfeebce2300e) )
	ROM_LOAD( "showd1p.bin",   0x18000, 0x2000, CRC(862b350d) SHA1(12e6c92ba424df578eac5a820a68aaaffd73c577) )
	ROM_LOAD( "showd1r.bin",   0x1a000, 0x2000, CRC(95b099ed) SHA1(1327852712ade3fc96bd8192045c081c4d32f4ba) )
	ROM_LOAD( "showd1s.bin",   0x1c000, 0x2000, CRC(8f230881) SHA1(daa8efc355fb042b2fce89a0d2950a90e56a806f) )
	ROM_LOAD( "showd1t.bin",   0x1e000, 0x2000, CRC(70e724c7) SHA1(df2905f91038693c87452813216aa86bbb81521b) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( yukon )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "yul-20.1a",  0x08000, 0x2000, CRC(d8929303) SHA1(29a4b1b44016584ca99e4744b0f5cf49c635584a) )
	ROM_LOAD( "yul-1.3a",   0x0a000, 0x2000, CRC(165fd218) SHA1(bc8b759f7753c91b694e9c38577292e040a8612f) )
	ROM_LOAD( "yul-1.4a",   0x0c000, 0x2000, CRC(308232ce) SHA1(5b54660ae7080be6c8795c597100923da09e755c) )
	ROM_LOAD( "yul-1.6a",   0x0e000, 0x2000, CRC(7ddd6235) SHA1(a946783627ffa3223ef40271e8326fdee13b3b4b) )
	ROM_LOAD( "yul-1.4d",   0x24000, 0x2000, CRC(bcd5676e) SHA1(c4e92546058746966fb42b95c601b93ebe5251f0) )
	ROM_LOAD( "yul-1.6d",   0x26000, 0x2000, CRC(283c1e25) SHA1(8272849964a7d82f8caa91189e818794eaa886f7) )
	ROM_LOAD( "yul-1.7d",   0x28000, 0x2000, CRC(7d03232a) SHA1(dd7b40f355fc6727805a62da00d721077c354462) )
	ROM_LOAD( "yul-1.8d",   0x2a000, 0x2000, CRC(a46a0253) SHA1(417b0c616671a68cb532637237e6b46a8e65d402) )
	ROM_LOAD( "yul-1.10d",  0x2c000, 0x2000, CRC(9e2ca5b3) SHA1(5a286f55598de0e99f00584cabdba3a35491d0f2) )
	ROM_LOAD( "yul-1.11d",  0x2e000, 0x2000, CRC(1d9f0981) SHA1(a58f113a87cad01ae2fb3257dd69739b4d3792c7) )
	ROM_LOAD( "yul-1.1c",   0x30000, 0x2000, CRC(dc1a8ce7) SHA1(bfa511232fe9533bc5cfdbb4b87a8c1da5f33acb) )
	ROM_LOAD( "yul-1.3c",   0x32000, 0x2000, CRC(ce840607) SHA1(c2b9da1976b3af8ac9fdacd878a72d89ff3db8fe) )
	ROM_LOAD( "yul-1.4c",   0x34000, 0x2000, CRC(f0c736ae) SHA1(73a6bcbe0a91300502da8eaf7196587c32002d03) )
	ROM_LOAD( "yul-1.6c",   0x36000, 0x2000, CRC(48779436) SHA1(413734020c07d8d8a59ec73f492d057e226042af) )
	ROM_LOAD( "yul-1.7c",   0x38000, 0x2000, CRC(b653ab9e) SHA1(8de6855244e0aacb689626bda02047cd5e3a6b1a) )
	ROM_LOAD( "yul-1.8c",   0x3a000, 0x2000, CRC(3e291d7e) SHA1(7d6287e138473b59f731b06775025b3552a34883) )
	ROM_LOAD( "yul-1.10c",  0x3c000, 0x2000, CRC(7f677082) SHA1(61910492d638a1d254c8903bd5c2038c523a1e2b) )
	ROM_LOAD( "yul-1.11c",  0x3e000, 0x2000, CRC(b7f5ea8d) SHA1(2e4831574323fe496d98ed661ce9264f897d6162) )
	ROM_LOAD( "yul-1.1b",   0x40000, 0x2000, CRC(75cb768d) SHA1(bbcb2e09370937ea476829940a0a41b0f505b315) )
	ROM_LOAD( "yul-1.3b",   0x42000, 0x2000, CRC(b76c4ec9) SHA1(d671dc399adc6e07c30636224d9679c1a4ddfc41) )
	ROM_LOAD( "yul-1.4b",   0x44000, 0x2000, CRC(1b981516) SHA1(16c417c9c1918a00dee976f3513925f8f28e6f41) )
	ROM_LOAD( "yul-1.6b",   0x46000, 0x2000, CRC(ab7b77e2) SHA1(40bc7dc10f06c4297b5e44b05ebcdcb5c1448072) )
	ROM_LOAD( "yul-1.7b",   0x48000, 0x2000, CRC(30a62d8f) SHA1(8b2cefd5c7393ec238d2d7b53320c08cce43c93b) )
	ROM_LOAD( "yul-1.8b",   0x4a000, 0x2000, CRC(fa85b58e) SHA1(11c18bff9f473281bcf6677ffffd499496af7b9d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "yua-1.1h",   0x0e000, 0x2000, CRC(f0df665a) SHA1(1fac03007563f569fdf57d5b16a0501e9a4dff01) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "yua-1.2k",   0x00000, 0x2000, CRC(67a86f7f) SHA1(a4f70aab90acd2502e8f3f39efdafcd71b1a22b4) )
	ROM_LOAD( "yua-1.2l",   0x02000, 0x2000, CRC(0bb8874b) SHA1(8b643dbd5412a713b3e2831dd1ba2b7d1f613ac2) )
	ROM_LOAD( "yua-1.2m",   0x04000, 0x2000, CRC(8b77eac8) SHA1(d70038cd6655e71c6488c555ecb1d1a424d00d49) )
	ROM_LOAD( "yua-1.2n",   0x06000, 0x2000, CRC(78e6eed6) SHA1(d9218745fc497a67373b0f6fd82caeef33bb4b3e) )
	ROM_LOAD( "yua-1.2p",   0x08000, 0x2000, CRC(a8fa14b7) SHA1(6687bc7c790f050da1b4f36a75e257c2b6a7d59e) )
	ROM_LOAD( "yua-1.2r",   0x0a000, 0x2000, CRC(ca978863) SHA1(cb45b89df4dbaff7191bdab84ac5c0ba16d9d314) )
	ROM_LOAD( "yua-1.2s",   0x0c000, 0x2000, CRC(74d51be4) SHA1(4368ed0a5d889f879e4059ebea039947c6e874cb) )
	ROM_LOAD( "yua-1.2t",   0x0e000, 0x2000, CRC(e8a9543e) SHA1(32b0a57bb2b9fd251ff3ba621960a1851c4098fd) )
	ROM_LOAD( "yua-1.1k",   0x10000, 0x2000, CRC(4e1f4f15) SHA1(cf075e09eaeab1e630b5b2e28c806d4c34eece48) )
	ROM_LOAD( "yua-1.1l",   0x12000, 0x2000, CRC(6779a745) SHA1(de7d8e39f053eaa45238844e477732295d9af494) )
	ROM_LOAD( "yua-1.1m",   0x14000, 0x2000, CRC(9cebd8ea) SHA1(ff8121c16e8fe93a59c49c20459287f6b002bbbc) )
	ROM_LOAD( "yua-1.1n",   0x16000, 0x2000, CRC(689d8a3f) SHA1(aa592b9edcac8d264c9c89871283dfeebce2300e) )
	ROM_LOAD( "yua-1.1p",   0x18000, 0x2000, CRC(862b350d) SHA1(12e6c92ba424df578eac5a820a68aaaffd73c577) )
	ROM_LOAD( "yua-1.1r",   0x1a000, 0x2000, CRC(95b099ed) SHA1(1327852712ade3fc96bd8192045c081c4d32f4ba) )
	ROM_LOAD( "yua-1.1s",   0x1c000, 0x2000, CRC(8f230881) SHA1(daa8efc355fb042b2fce89a0d2950a90e56a806f) )
	ROM_LOAD( "yua-1.1t",   0x1e000, 0x2000, CRC(80926a5c) SHA1(6f9fb18ffb131f5eccade104a289c2d3032bac78) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END


ROM_START( yukon1 )
	ROM_REGION( 0x50000, "maincpu", 0 )     /* 64k for code for the first CPU, plus lots of banked ROMs */
	ROM_LOAD( "yul-1.1a",   0x08000, 0x2000, CRC(0286411b) SHA1(46f3335cb78458fab44e7976ab0c4ed318626ab3) )
	ROM_LOAD( "yul-1.3a",   0x0a000, 0x2000, CRC(165fd218) SHA1(bc8b759f7753c91b694e9c38577292e040a8612f) )
	ROM_LOAD( "yul-1.4a",   0x0c000, 0x2000, CRC(308232ce) SHA1(5b54660ae7080be6c8795c597100923da09e755c) )
	ROM_LOAD( "yul-1.6a",   0x0e000, 0x2000, CRC(7ddd6235) SHA1(a946783627ffa3223ef40271e8326fdee13b3b4b) )
	ROM_LOAD( "yul-1.4d",   0x24000, 0x2000, CRC(bcd5676e) SHA1(c4e92546058746966fb42b95c601b93ebe5251f0) )
	ROM_LOAD( "yul-1.6d",   0x26000, 0x2000, CRC(283c1e25) SHA1(8272849964a7d82f8caa91189e818794eaa886f7) )
	ROM_LOAD( "yul-1.7d",   0x28000, 0x2000, CRC(7d03232a) SHA1(dd7b40f355fc6727805a62da00d721077c354462) )
	ROM_LOAD( "yul-1.8d",   0x2a000, 0x2000, CRC(a46a0253) SHA1(417b0c616671a68cb532637237e6b46a8e65d402) )
	ROM_LOAD( "yul-1.10d",  0x2c000, 0x2000, CRC(9e2ca5b3) SHA1(5a286f55598de0e99f00584cabdba3a35491d0f2) )
	ROM_LOAD( "yul-1.11d",  0x2e000, 0x2000, CRC(1d9f0981) SHA1(a58f113a87cad01ae2fb3257dd69739b4d3792c7) )
	ROM_LOAD( "yul-1.1c",   0x30000, 0x2000, CRC(dc1a8ce7) SHA1(bfa511232fe9533bc5cfdbb4b87a8c1da5f33acb) )
	ROM_LOAD( "yul-1.3c",   0x32000, 0x2000, CRC(ce840607) SHA1(c2b9da1976b3af8ac9fdacd878a72d89ff3db8fe) )
	ROM_LOAD( "yul-1.4c",   0x34000, 0x2000, CRC(f0c736ae) SHA1(73a6bcbe0a91300502da8eaf7196587c32002d03) )
	ROM_LOAD( "yul-1.6c",   0x36000, 0x2000, CRC(48779436) SHA1(413734020c07d8d8a59ec73f492d057e226042af) )
	ROM_LOAD( "yul-1.7c",   0x38000, 0x2000, CRC(b653ab9e) SHA1(8de6855244e0aacb689626bda02047cd5e3a6b1a) )
	ROM_LOAD( "yul-1.8c",   0x3a000, 0x2000, CRC(3e291d7e) SHA1(7d6287e138473b59f731b06775025b3552a34883) )
	ROM_LOAD( "yul-1.10c",  0x3c000, 0x2000, CRC(7f677082) SHA1(61910492d638a1d254c8903bd5c2038c523a1e2b) )
	ROM_LOAD( "yul-1.11c",  0x3e000, 0x2000, CRC(b7f5ea8d) SHA1(2e4831574323fe496d98ed661ce9264f897d6162) )
	ROM_LOAD( "yul-1.1b",   0x40000, 0x2000, CRC(75cb768d) SHA1(bbcb2e09370937ea476829940a0a41b0f505b315) )
	ROM_LOAD( "yul-1.3b",   0x42000, 0x2000, CRC(b76c4ec9) SHA1(d671dc399adc6e07c30636224d9679c1a4ddfc41) )
	ROM_LOAD( "yul-1.4b",   0x44000, 0x2000, CRC(1b981516) SHA1(16c417c9c1918a00dee976f3513925f8f28e6f41) )
	ROM_LOAD( "yul-1.6b",   0x46000, 0x2000, CRC(ab7b77e2) SHA1(40bc7dc10f06c4297b5e44b05ebcdcb5c1448072) )
	ROM_LOAD( "yul-1.7b",   0x48000, 0x2000, CRC(30a62d8f) SHA1(8b2cefd5c7393ec238d2d7b53320c08cce43c93b) )
	ROM_LOAD( "yul-1.8b",   0x4a000, 0x2000, CRC(fa85b58e) SHA1(11c18bff9f473281bcf6677ffffd499496af7b9d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "yua-1.1h",   0x0e000, 0x2000, CRC(f0df665a) SHA1(1fac03007563f569fdf57d5b16a0501e9a4dff01) )

	ROM_REGION( 0x20000, "cvsd", 0 )
	ROM_LOAD( "yua-1.2k",   0x00000, 0x2000, CRC(67a86f7f) SHA1(a4f70aab90acd2502e8f3f39efdafcd71b1a22b4) )
	ROM_LOAD( "yua-1.2l",   0x02000, 0x2000, CRC(0bb8874b) SHA1(8b643dbd5412a713b3e2831dd1ba2b7d1f613ac2) )
	ROM_LOAD( "yua-1.2m",   0x04000, 0x2000, CRC(8b77eac8) SHA1(d70038cd6655e71c6488c555ecb1d1a424d00d49) )
	ROM_LOAD( "yua-1.2n",   0x06000, 0x2000, CRC(78e6eed6) SHA1(d9218745fc497a67373b0f6fd82caeef33bb4b3e) )
	ROM_LOAD( "yua-1.2p",   0x08000, 0x2000, CRC(a8fa14b7) SHA1(6687bc7c790f050da1b4f36a75e257c2b6a7d59e) )
	ROM_LOAD( "yua-1.2r",   0x0a000, 0x2000, CRC(ca978863) SHA1(cb45b89df4dbaff7191bdab84ac5c0ba16d9d314) )
	ROM_LOAD( "yua-1.2s",   0x0c000, 0x2000, CRC(74d51be4) SHA1(4368ed0a5d889f879e4059ebea039947c6e874cb) )
	ROM_LOAD( "yua-1.2t",   0x0e000, 0x2000, CRC(e8a9543e) SHA1(32b0a57bb2b9fd251ff3ba621960a1851c4098fd) )
	ROM_LOAD( "yua-1.1k",   0x10000, 0x2000, CRC(4e1f4f15) SHA1(cf075e09eaeab1e630b5b2e28c806d4c34eece48) )
	ROM_LOAD( "yua-1.1l",   0x12000, 0x2000, CRC(6779a745) SHA1(de7d8e39f053eaa45238844e477732295d9af494) )
	ROM_LOAD( "yua-1.1m",   0x14000, 0x2000, CRC(9cebd8ea) SHA1(ff8121c16e8fe93a59c49c20459287f6b002bbbc) )
	ROM_LOAD( "yua-1.1n",   0x16000, 0x2000, CRC(689d8a3f) SHA1(aa592b9edcac8d264c9c89871283dfeebce2300e) )
	ROM_LOAD( "yua-1.1p",   0x18000, 0x2000, CRC(862b350d) SHA1(12e6c92ba424df578eac5a820a68aaaffd73c577) )
	ROM_LOAD( "yua-1.1r",   0x1a000, 0x2000, CRC(95b099ed) SHA1(1327852712ade3fc96bd8192045c081c4d32f4ba) )
	ROM_LOAD( "yua-1.1s",   0x1c000, 0x2000, CRC(8f230881) SHA1(daa8efc355fb042b2fce89a0d2950a90e56a806f) )
	ROM_LOAD( "yua-1.1t",   0x1e000, 0x2000, CRC(80926a5c) SHA1(6f9fb18ffb131f5eccade104a289c2d3032bac78) )

	ROM_REGION( 0x00900, "user1", 0 )
	/* vertical sync timing */
	ROM_LOAD( "xbl.12h",    0x00000, 0x0100, CRC(375c8bfc) SHA1(2602dde6961cc6b63d1652e2f3e4cfae2d8a34d9) )
	/* horizontal sync timing */
	ROM_LOAD( "xbl.9h",     0x00100, 0x0100, CRC(2e7d5562) SHA1(7cd51fad8236b9853eff2eb84b474838ae1b44e8) )
	ROM_LOAD( "xbl.2h",     0x00200, 0x0100, CRC(b078c1e4) SHA1(13834da4384ad43bc1671366fd428520cc3d1c1a) )
	ROM_LOAD( "xml-3k_mmi_6331.bin", 0x00300, 0x0020, CRC(afa289d1) SHA1(703f3e433ebe0b9c2f1be31bef0d01b8007d48ea) )
	ROM_LOAD( "xbl.4k",     0x00400, 0x0100, CRC(31a9549c) SHA1(5bfba7ef3f3f5fc59bc03feca39bb16d54a92778) )
	ROM_LOAD( "xbl.5k",     0x00500, 0x0100, CRC(1379bb2a) SHA1(51e9e21aeb0db8727f58fda708ddea8fb53378d9) )
	ROM_LOAD( "xbl.6k",     0x00600, 0x0100, CRC(588969f7) SHA1(316db275c4026e3a24e44f39f160e10189d310a3) )
	ROM_LOAD( "xbl.7k",     0x00700, 0x0100, CRC(eda360b8) SHA1(79d84207e28c1289210cebd96abad6cfe1b4c1d0) )
	ROM_LOAD( "xbl.8k",     0x00800, 0x0100, CRC(9d434cb1) SHA1(c3390bc1c02fe74ff2067f7fccfd1ad2c30b54a9) )
ROM_END



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

DRIVER_INIT_MEMBER(exidy440_state,exidy440)
{
	m_showdown_bank_data[0] = m_showdown_bank_data[1] = NULL;
}


DRIVER_INIT_MEMBER(exidy440_state,claypign)
{
	DRIVER_INIT_CALL(exidy440);
	m_maincpu->space(AS_PROGRAM).install_read_handler(0x2ec0, 0x2ec3, read8_delegate(FUNC(exidy440_state::claypign_protection_r),this));
}


DRIVER_INIT_MEMBER(exidy440_state,topsecex)
{
	DRIVER_INIT_CALL(exidy440);

	/* extra input ports and scrolling */
	m_maincpu->space(AS_PROGRAM).install_read_handler(0x2ec5, 0x2ec5, read8_delegate(FUNC(exidy440_state::topsecex_input_port_5_r),this));
	m_maincpu->space(AS_PROGRAM).install_read_port(0x2ec6, 0x2ec6, "AN0");
	m_maincpu->space(AS_PROGRAM).install_read_port(0x2ec7, 0x2ec7, "IN4");

	m_topsecex_yscroll = m_maincpu->space(AS_PROGRAM).install_write_handler(0x2ec1, 0x2ec1, write8_delegate(FUNC(exidy440_state::topsecex_yscroll_w),this));
}


DRIVER_INIT_MEMBER(exidy440_state,showdown)
{
	static const UINT8 bankdata0[0x18] =
	{
		0x15,0x40,0xc1,0x8d,0x4c,0x84,0x0e,0xce,
		0x52,0xd0,0x99,0x48,0x80,0x09,0xc9,0x45,
		0xc4,0x8e,0x5a,0x92,0x18,0xd8,0x51,0xc0
	};
	static const UINT8 bankdata1[0x18] =
	{
		0x11,0x51,0xc0,0x89,0x4d,0x85,0x0c,0xcc,
		0x46,0xd2,0x98,0x59,0x91,0x08,0xc8,0x41,
		0xc5,0x8c,0x4e,0x86,0x1a,0xda,0x50,0xd1
	};

	DRIVER_INIT_CALL(exidy440);

	/* set up the fake PLD */
	m_showdown_bank_data[0] = bankdata0;
	m_showdown_bank_data[1] = bankdata1;
}


DRIVER_INIT_MEMBER(exidy440_state,yukon)
{
	static const UINT8 bankdata0[0x18] =
	{
		0x31,0x40,0xc1,0x95,0x54,0x90,0x16,0xd6,
		0x62,0xe0,0xa5,0x44,0x80,0x05,0xc5,0x51,
		0xd0,0x96,0x66,0xa2,0x24,0xe4,0x61,0xc0
	};
	static const UINT8 bankdata1[0x18] =
	{
		0x21,0x61,0xc0,0x85,0x55,0x91,0x14,0xd4,
		0x52,0xe2,0xa4,0x65,0xa1,0x04,0xc4,0x41,
		0xd1,0x94,0x56,0x92,0x26,0xe6,0x60,0xe1
	};

	DRIVER_INIT_CALL(exidy440);

	/* set up the fake PLD */
	m_showdown_bank_data[0] = bankdata0;
	m_showdown_bank_data[1] = bankdata1;
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1983, crossbow, 0,        exidy440, crossbow, exidy440_state, exidy440, ROT0, "Exidy", "Crossbow (version 2.0)", 0 )
GAME( 1984, cheyenne, 0,        exidy440, cheyenne, exidy440_state, exidy440, ROT0, "Exidy", "Cheyenne (version 1.0)", 0 )
GAME( 1985, combat,   0,        exidy440, combat, exidy440_state,   exidy440, ROT0, "Exidy", "Combat (version 3.0)", 0 )
GAME( 1985, catch22,  combat,   exidy440, catch22, exidy440_state,  exidy440, ROT0, "Exidy", "Catch-22 (version 8.0)", 0 )
GAME( 1985, cracksht, 0,        exidy440, cracksht, exidy440_state, exidy440, ROT0, "Exidy", "Crackshot (version 2.0)", 0 )
GAME( 1986, claypign, 0,        exidy440, claypign, exidy440_state, claypign, ROT0, "Exidy", "Clay Pigeon (version 2.0)", 0 )
GAME( 1986, chiller,  0,        exidy440, chiller, exidy440_state,  exidy440, ROT0, "Exidy", "Chiller (version 3.0)", 0 )
GAME( 1986, topsecex, 0,        topsecex, topsecex, exidy440_state, topsecex, ROT0, "Exidy", "Top Secret (Exidy) (version 1.0)", 0 )
GAME( 1987, hitnmiss, 0,        exidy440, hitnmiss, exidy440_state, exidy440, ROT0, "Exidy", "Hit 'n Miss (version 3.0)", 0 )
GAME( 1987, hitnmiss2,hitnmiss, exidy440, hitnmiss, exidy440_state, exidy440, ROT0, "Exidy", "Hit 'n Miss (version 2.0)", 0 )
GAME( 1988, whodunit, 0,        exidy440, whodunit, exidy440_state, exidy440, ROT0, "Exidy", "Who Dunit (version 9.0)", 0 )
GAME( 1988, whodunit8,whodunit, exidy440, whodunit, exidy440_state, exidy440, ROT0, "Exidy", "Who Dunit (version 8.0)", 0 )
GAME( 1988, showdown, 0,        exidy440, showdown, exidy440_state, showdown, ROT0, "Exidy", "Showdown (version 5.0)", 0 )
GAME( 1989, yukon,    0,        exidy440, showdown, exidy440_state, yukon,    ROT0, "Exidy", "Yukon (version 2.0)", 0 )
GAME( 1989, yukon1,   yukon,    exidy440, showdown, exidy440_state, yukon,    ROT0, "Exidy", "Yukon (version 1.0)", 0 )
