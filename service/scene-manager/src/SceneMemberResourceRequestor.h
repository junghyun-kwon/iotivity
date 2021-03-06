/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef SM_SCENEMEMBER_RESOURCE_REQUESTOR_H_
#define SM_SCENEMEMBER_RESOURCE_REQUESTOR_H_

#include "SceneCommons.h"
#include "RCSRemoteResourceObject.h"

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {

        class SceneMemberResourceRequestor
            : public std::enable_shared_from_this< SceneMemberResourceRequestor >
        {
            public:
                typedef std::shared_ptr< SceneMemberResourceRequestor > Ptr;
                typedef std::weak_ptr< SceneMemberResourceRequestor > wPtr;

                typedef std::function < void(int eCode) > InternalAddSceneActionCallback;

            public:
                /**
                 * constructor
                 * @param memberResource   member resource
                 * @param id               member id
                 */
                SceneMemberResourceRequestor(RCSRemoteResourceObject::Ptr memberResource,
                                             const std::string &id);
                ~SceneMemberResourceRequestor() = default;

                /**
                 * This method is for scene action creation request
                 * @param sceneName  name of the scene
                 * @param attr       resource attributes
                 * @param InternalAddSceneActionCallback  callback for scene action
                 */
                void requestSceneActionCreation(const std::string &sceneName,
                                                const RCSResourceAttributes &attr,
                                                InternalAddSceneActionCallback);

                /** This method is for get request */
                void requestGet(const std::string &, RCSRemoteResourceObject::GetCallback);
                /** This method is for set remote resource object */
                void setRemoteResourceObject(RCSRemoteResourceObject::Ptr);
                /** This method is for get remote resource object */
                RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;

            private:
                static void onSceneActionCreated(
                    const RCSResourceAttributes &, int eCode,
                    const std::string &sceneName, const RCSResourceAttributes &requestedAttrs,
                    const InternalAddSceneActionCallback &, SceneMemberResourceRequestor::wPtr);

                void onSceneActionCreated_impl(
                    const RCSResourceAttributes &, int eCode,
                    const std::string &sceneName, const RCSResourceAttributes &requestedAttrs,
                    const InternalAddSceneActionCallback &);

            private:
                std::string m_id;
                RCSRemoteResourceObject::Ptr m_remoteResource;
                RCSRemoteResourceObject::Ptr m_sceneMemberResource;
        };

    }
}

#endif /* SM_SCENEMEMBER_RESOURCE_REQUESTOR_H_ */

