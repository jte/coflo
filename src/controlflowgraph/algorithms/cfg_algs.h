/*
 * Copyright 2012 Gary R. Van Sickle (grvs@users.sourceforge.net).
 *
 * This file is part of CoFlo.
 *
 * CoFlo is free software: you can redistribute it and/or modify it under the
 * terms of version 3 of the GNU General Public License as published by the Free
 * Software Foundation.
 *
 * CoFlo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * CoFlo.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file */

#ifndef CFG_ALGS_H
#define CFG_ALGS_H

class ControlFlowGraph;
class Function;

/**
 * Traverses the CFG of Function @a f and marks all back edges as such.
 *
 * @param g Reference to the ControlFlowGraph to process.
 * @param f
 */
void FixupBackEdges(ControlFlowGraph &g);

void InsertMergeNodes(Function *f);

void SplitCriticalEdges(Function *f);

void StructureCompoundConditionals(Function *f);

void RemoveRedundantNodes(ControlFlowGraph *g);

#endif // CFG_ALGS_H
