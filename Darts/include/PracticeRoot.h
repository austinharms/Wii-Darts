#ifndef DARTS_PRACTICE_ROOT_H_
#define DARTS_PRACTICE_ROOT_H_
#include "engine/Core.h"
#include "engine/RootEntity.h"
class PracticeRoot : public RootEntity
{
public:
	PracticeRoot();
	virtual ~PracticeRoot();

protected:
	void OnLoad() WD_OVERRIDE;
	void OnUpdate() WD_OVERRIDE;
	void OnUnload() WD_OVERRIDE;

private:
	struct SceneData;
	SceneData& GetData();
	const SceneData& GetData() const;
};
#endif // !DARTS_PRACTICE_ROOT_H_