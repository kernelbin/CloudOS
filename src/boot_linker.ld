OUTPUT_ARCH("i386")
OUTPUT_FORMAT("binary")

SECTIONS
{
	.text 0xbe00 :
	{
		*(.text16)
		*(.text32)
	}
}