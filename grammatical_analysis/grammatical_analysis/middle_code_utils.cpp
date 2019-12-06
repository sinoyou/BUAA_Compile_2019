#include "Quaternary.h"
#include "debug.h"

QuaterType cmp_reverse(QuaterType cmp_type) {
	switch (cmp_type)
	{
	case EqlCmp:
		return NeqCmp;
	case NeqCmp:
		return EqlCmp;
	case GtCmp:
		return LeqCmp;
	case GeqCmp:
		return LtCmp;
	case LtCmp:
		return GeqCmp;
	case LeqCmp:
		return GtCmp;
	default:
		char buf[100];
		sprintf(buf, "[ERROR] Unexpected Compare Type in cmp_fit_from_bnz_to_bz %d", cmp_type);
		DEBUG_PRINT(buf);
	}
}