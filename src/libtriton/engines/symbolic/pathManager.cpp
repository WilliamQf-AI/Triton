//! \file
/*
**  Copyright (C) - Triton
**
**  This program is under the terms of the LGPLv3 License.
*/

#include <stdexcept>
#include <pathManager.hpp>



namespace triton {
  namespace engines {
    namespace symbolic {

      PathManager::PathManager() {
      }


      PathManager::PathManager(const PathManager &copy) {
        this->pathConstraints = copy.pathConstraints;
      }


      PathManager::~PathManager() {
      }


      /* Returns the logical conjunction vector of path constraint */
      std::vector<triton::engines::symbolic::PathConstraint>& PathManager::getPathConstraints(void) {
        return this->pathConstraints;
      }


      /* Returns the logical conjunction AST of path constraint */
      triton::ast::AbstractNode* PathManager::getPathConstraintsAst(void) {
        std::vector<triton::engines::symbolic::PathConstraint>::iterator it;
        triton::ast::AbstractNode* node = nullptr;

        /* by default PC is T (top) */
        node = triton::ast::equal(
                 triton::ast::bvtrue(),
                 triton::ast::bvtrue()
               );

        /* Then, we create a conjunction of pc */
        for (it = this->pathConstraints.begin(); it != this->pathConstraints.end(); it++) {
          node = triton::ast::land(node, it->getTakenPathConstraintAst());
        }

        return node;
      }


      triton::uint32 PathManager::getNumberOfPathConstraints(void) {
        return this->pathConstraints.size();
      }


      /* Add a path constraint */
      void PathManager::addPathConstraint(triton::ast::AbstractNode* pc) {
        triton::engines::symbolic::PathConstraint pco;
        triton::__uint targetBb = 0;
        triton::uint32 size = 0;

        if (pc == nullptr)
          throw std::runtime_error("PathManager::addPathConstraint(): The PC node cannot be null.");

        /* Basic block taken */
        targetBb = pc->evaluate().convert_to<triton::__uint>();
        size     = pc->getBitvectorSize();

        if (size == 0)
          throw std::runtime_error("PathManager::addPathConstraint(): The PC node size cannot be zero.");

        /* Multiple branches */
        if (pc->getKind() == triton::ast::ITE_NODE) {
          triton::__uint bb1 = pc->getChilds()[1]->evaluate().convert_to<triton::__uint>();
          triton::__uint bb2 = pc->getChilds()[2]->evaluate().convert_to<triton::__uint>();

          triton::ast::AbstractNode* bb1pc = (bb1 == targetBb) ? triton::ast::equal(pc, triton::ast::bv(targetBb, size)) :
                                                                 triton::ast::lnot(triton::ast::equal(pc, triton::ast::bv(targetBb, size)));

          triton::ast::AbstractNode* bb2pc = (bb2 == targetBb) ? triton::ast::equal(pc, triton::ast::bv(targetBb, size)) :
                                                                 triton::ast::lnot(triton::ast::equal(pc, triton::ast::bv(targetBb, size)));

          pco.addBranchConstraint(bb1 == targetBb, bb1, bb1pc);
          pco.addBranchConstraint(bb2 == targetBb, bb2, bb2pc);

          this->pathConstraints.push_back(pco);
        }

        /* Direct branch */
        else {
          pco.addBranchConstraint(true, targetBb, triton::ast::equal(pc, triton::ast::bv(targetBb, size)));
          this->pathConstraints.push_back(pco);
        }

      }


      void PathManager::clearPathConstraints(void) {
        this->pathConstraints.clear();
      }

    }; /* symbolic namespace */
  }; /* engines namespace */
}; /*triton namespace */
