#ifndef H_UnboundVariableManager
#define H_UnboundVariableManager

#include "FlawManager.hh"
#include "ConstraintEngineListener.hh"
#include "PlanDatabaseListener.hh"
#include "UnboundVariableDecisionPoint.hh"

/**
 * @brief Provides class declaration for handling variable flaws.
 * @author Conor McGann
 */
namespace EUROPA {
  namespace SOLVERS {

    class UnboundVariableManager: public FlawManager {
    public:
      UnboundVariableManager(const TiXmlElement& configData);

      virtual ~UnboundVariableManager();

      bool inScope(const EntityId& entity) const;

    private:
      virtual DecisionPointId next(unsigned int priorityLowerBound, unsigned int& bestPriority);

      virtual DecisionPointId next(unsigned int priorityLowerBound,
				   unsigned int& bestPriority,
				   const ConstrainedVariableSet& flawCandidates);
      void handleInitialize();
      void addFlaw(const ConstrainedVariableId& var);
      void removeFlaw(const ConstrainedVariableId& var);
      void toggleSingletonFlaw(const ConstrainedVariableId& var);
      void addGuard(const ConstrainedVariableId& var);
      void removeGuard(const ConstrainedVariableId& var);
      void handleConstraintAddition(const ConstraintId& constraint);
      void handleConstraintRemoval(const ConstraintId& constraint);

      /**
       * @brief Utility to test if the given variable is part of a token that is merged, rejected or inactive.
       */
      static bool variableOfNonActiveToken(const ConstrainedVariableId& var);

      ConstrainedVariableSet m_flawCandidates; /*!< All variables that have passed the static filter */
      ConstrainedVariableSet m_singletonFlawCandidates; /*!< All singleton variables that have passed the static filter */
      std::map<ConstrainedVariableId, unsigned int> m_guardCache; /*!< Cache of variables that are
								    guarded. Includes reference counts. */

      /**
       * @brief Plugs manager into ConstraintEngine events to
       * synchronize flaw candidates
       */
      class CeListener: public ConstraintEngineListener {
      public:
	CeListener(const ConstraintEngineId& ce, 
		   UnboundVariableManager& dm);

	void notifyRemoved(const ConstrainedVariableId& variable);
	void notifyChanged(const ConstrainedVariableId& variable, const DomainListener::ChangeType& changeType);
	void notifyAdded(const ConstraintId& constraint);
	void notifyRemoved(const ConstraintId& constraint);

      private:
	UnboundVariableManager& m_fm;
      };

      friend class UnboundVariableManager::CeListener;
      UnboundVariableManager::CeListener* m_ceListener; /*!< For Processing constraint engine events */

      /**
       * @brief Plugs into Token Activation and Deactivation events on the Plan
       * Database to synchronize flaw candidates.
       */
      class DbListener: public PlanDatabaseListener {
      public:
	DbListener(const PlanDatabaseId& db,
		   UnboundVariableManager& dm);
      private:
	void notifyActivated(const TokenId& token);
	void notifyDeactivated(const TokenId& token);
	UnboundVariableManager& m_fm;
      };

      friend class UnboundVariableManager::DbListener;
      UnboundVariableManager::DbListener* m_dbListener; /*!< For processing Plan Database events */
    };
  }
}
#endif
