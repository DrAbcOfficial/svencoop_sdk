// DetourNavMesh.cpp
//
// Based on Detour/Recast (public domain)
//

#include "DetourNavMesh.h"

#include <math.h>
#include <string.h>

dtPolyRef dtNavMesh::getPolyRefBase(const dtMeshTile* tile) const
{
	if (tile)
		return (dtPolyRef)((tile - m_tiles) / sizeof(dtMeshTile)) << m_params.polyBits |
			(dtPolyRef)tile->salt << (m_params.polyBits + m_params.tileBits);
	return 0;
}

dtPolyRef dtNavMesh::getTileRef(const dtMeshTile* tile) const
{
	return getPolyRefBase(tile);
}

void dtNavMesh::calcTileLoc(const float* pos, int* tx, int* ty) const
{
	*tx = (int)floorf((pos[0] - m_params.origin[0]) / m_params.tileSize);
	*ty = (int)floorf((pos[2] - m_params.origin[2]) / m_params.tileSize);
}

dtStatus dtNavMesh::getTileAndPolyByRef(dtPolyRef ref, const dtMeshTile** tile, const dtPoly** poly) const
{
	if (!ref)
		return DT_FAILURE;

	const unsigned int tileBits = m_params.tileBits;
	const unsigned int polyBits = m_params.polyBits;
	const unsigned int saltBits = m_params.saltBits;

	const unsigned int tileIdx = (ref >> (polyBits + saltBits)) & ((1 << tileBits) - 1);
	const unsigned int polyIdx = ref & ((1 << polyBits) - 1);
	const unsigned int salt = (ref >> polyBits) & ((1 << saltBits) - 1);

	if (tileIdx >= (unsigned int)m_params.maxTiles)
		return DT_INVALID_STATE;

	const dtMeshTile* targetTile = &m_tiles[tileIdx];

	if (targetTile->salt != salt)
		return DT_INVALID_STATE;

	if (targetTile->header == 0)
		return DT_INVALID_STATE;

	if (polyIdx >= (unsigned int)targetTile->header->polyCount)
		return DT_INVALID_STATE;

	*tile = targetTile;
	*poly = &targetTile->polys[polyIdx];
	return DT_SUCCESS;
}

int dtNavMesh::getTilesAt(int x, int y, const dtMeshTile** tiles, int maxTiles) const
{
	int n = 0;
	const unsigned int hash = (unsigned int)x * 0x8da6b343 + (unsigned int)y * 0xd8163841;
	const dtMeshTile* t = m_gridBuckets[hash & m_gridHashSize];
	while (t)
	{
		if (t->header && x == t->header->tileX && y == t->header->tileY && n < maxTiles)
		{
			tiles[n++] = t;
		}
		t = t->next;
	}
	return n;
}

int dtNavMesh::getTileStateSize(const dtMeshTile* tile) const
{
	if (!tile)
		return 0;
	return 4 * tile->header->polyCount + sizeof(dtTileState);
}

void dtNavMesh::connectIntLinks(dtMeshTile* tile)
{
	if (tile && tile->header)
	{
		dtMeshHeader* header = tile->header;
		const dtPolyRef base = getPolyRefBase(tile);
		dtPoly* polys = tile->polys;
		dtLink* links = tile->links;

		for (int i = 0; i < header->polyCount; ++i)
		{
			dtPoly* poly = polys + i;
			poly->firstLink = DT_NULL_LINK;
			if (poly->polyType == DT_POLYTYPE_GROUND)
			{
				for (int j = poly->neighbCnt - 1; j >= 0; --j)
				{
					if (poly->refs[j] != DT_NULL_LINK)
					{
						if (tile->firstLink != DT_NULL_LINK)
						{
							dtLink* link = links + tile->firstLink;
							const unsigned int firstLink = tile->firstLink;
							tile->firstLink = link->next;
							link->ref = base | (dtPolyRef)(poly->refs[j] - 1);
							link->edge = (unsigned char)j;
							link->internal = 0xff;
							link->pad[0] = 0;
							link->pad[1] = 0;
							link->next = poly->firstLink;
							poly->firstLink = firstLink;
						}
					}
				}
			}
		}
	}
}

dtStatus dtNavMesh::unconnectExtLinks(dtMeshTile* tile, dtMeshTile* prev)
{
	if (tile && prev)
	{
		const dtPolyRef base = getTileRef(prev);
		const unsigned int tileIdx = base >> m_params.polyBits;
		const unsigned int tileId = tileIdx & ((1 << m_params.tileBits) - 1);
		const dtMeshHeader* header = tile->header;
		dtPoly* polys = tile->polys;
		dtLink* links = tile->links;

		for (int i = 0; i < header->polyCount; ++i)
		{
			dtPoly* poly = polys + i;
			dtLink* link = poly->firstLink == DT_NULL_LINK ? 0 : links + poly->firstLink;
			dtLink* prevLink = 0;
			while (link)
			{
				if (link->internal != 0xff && ((link->ref >> m_params.polyBits) & ((1 << m_params.tileBits) - 1)) == (dtPolyRef)tileId)
				{
					const dtLink* next = link->next == DT_NULL_LINK ? 0 : links + link->next;
					if (prevLink)
						prevLink->next = link->next;
					else
						poly->firstLink = link->next;
					link->next = tile->firstLink;
					tile->firstLink = (unsigned int)(link - links);
					link = (dtLink *)next;
				}
				else
				{
					prevLink = link;
					link = link->next == DT_NULL_LINK ? 0 : links + link->next;
				}
			}
		}
	}
	return DT_SUCCESS;
}

void dtNodePool::clear()
{
	memset(m_next, 0xff, m_hashSize * sizeof(unsigned short));
	m_nodeCount = 0;
}
