//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "SceneListResource.h"

#include "RCSRequest.h"
#include "OCApi.h"

namespace OIC
{
    namespace Service
    {
        SceneListResource::SceneListResource()
        : m_sceneListName(), m_sceneListObj()
        {
            m_sceneListObj = RCSResourceObject::Builder(
                    SCENE_LIST_URI, SCENE_LIST_RT, OC_RSRVD_INTERFACE_DEFAULT).
                            addInterface(OC::BATCH_INTERFACE).
                            setDiscoverable(true).setObservable(false).build();

            m_sceneListObj->setAttribute(SCENE_KEY_NAME, SCENE_LIST_DEFAULT_NAME);
            m_sceneListObj->setAttribute(SCENE_KEY_RTS, SCENE_LIST_RT);

            m_sceneListObj->setSetRequestHandler(&SceneListRequestHandler::onSetRequest);
            m_sceneListObj->setGetRequestHandler(&SceneListRequestHandler::onGetRequest);
        }

        SceneListResource * SceneListResource::getInstance()
        {
            static SceneListResource instance;
            return & instance;
        }

        void SceneListResource::addSceneCollectionResource(
                SceneCollectionResource::Ptr newObject)
        {
            std::lock_guard<std::mutex> collectionlock(m_sceneCollectionLock);
            m_sceneCollections.push_back(newObject);
            m_sceneListObj->bindResource(newObject->getRCSResourceObject());
        }

        std::string SceneListResource::getName() const
        {
            return m_sceneListName;
        }

        void SceneListResource::setName(std::string && newName)
        {
            m_sceneListName = std::move(newName);

            RCSResourceObject::LockGuard guard(m_sceneListObj);
            m_sceneListObj->setAttribute(SCENE_KEY_NAME, m_sceneListName);
        }

        void SceneListResource::setName(const std::string & newName)
        {
            setName(std::string(newName));
        }

        std::vector<SceneCollectionResource::Ptr> SceneListResource::getSceneCollections() const
        {
            return m_sceneCollections;
        }

        std::vector<RCSResourceAttributes> SceneListResource::getChildrenAttributes() const
        {
            std::vector<RCSResourceAttributes> childrenAttrs;

            auto sceneCollections = getSceneCollections();

            std::for_each(sceneCollections.begin(), sceneCollections.end(),
                    [& childrenAttrs](const SceneCollectionResource::Ptr & pCollection)
                    {
                        RCSResourceAttributes collectionAttr;
                        {
                            RCSResourceObject::LockGuard guard(
                                    pCollection->getRCSResourceObject());
                            collectionAttr = pCollection->getRCSResourceObject()->getAttributes();
                        }

                        auto sceneMembers = pCollection->getSceneMembers();
                        std::vector<RCSResourceAttributes> membersAttrs;

                        std::for_each(sceneMembers.begin(), sceneMembers.end(),
                                [& membersAttrs](const SceneMemberResource::Ptr & pMember)
                                {
                                    RCSResourceObject::LockGuard guard(pMember->getRCSResourceObject());
                                    membersAttrs.push_back(pMember->getRCSResourceObject()->getAttributes());
                                });

                        if (membersAttrs.size())
                        {
                            collectionAttr[SCENE_KEY_CHILD] = membersAttrs;
                        }

                        childrenAttrs.push_back(collectionAttr);
                    });

            return childrenAttrs;
        }

        RCSSetResponse SceneListResource::SceneListRequestHandler::onSetRequest(
                const RCSRequest & request, RCSResourceAttributes & attributes)
        {
            if(request.getInterface() == LINK_BATCH)
            {
                auto newObject
                    = SceneCollectionResource::createSceneCollectionObject(attributes);

                SceneListResource::getInstance()->addSceneCollectionResource(newObject);

                auto responseAtt = attributes;
                responseAtt[SCENE_KEY_NAME] = RCSResourceAttributes::Value(newObject->getName());
                responseAtt[SCENE_KEY_ID] = RCSResourceAttributes::Value(newObject->getId());

                auto uri = COAP_TAG + newObject->getAddress() + newObject->getUri();
                responseAtt[SCENE_KEY_PAYLOAD_LINK]
                            = RCSResourceAttributes::Value(uri);

                return RCSSetResponse::create(responseAtt, SCENE_RESPONSE_SUCCESS).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            if (attributes.contains(SCENE_KEY_NAME))
            {
                SceneListResource::getInstance()->setName(
                        attributes.at(SCENE_KEY_NAME).get<std::string>());
                return RCSSetResponse::create(attributes, SCENE_RESPONSE_SUCCESS).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            return RCSSetResponse::create(attributes, SCENE_CLIENT_BADREQUEST).
                    setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSGetResponse SceneListResource::SceneListRequestHandler::onGetRequest(
                const RCSRequest & request, RCSResourceAttributes & attributes)
        {

            if(request.getInterface() != OC::DEFAULT_INTERFACE)
            {
                return RCSGetResponse::defaultAction();
            }

            auto childrenAttrs = SceneListResource::getInstance()->getChildrenAttributes();

            RCSResourceAttributes retAttr =  attributes;
            if (childrenAttrs.size())
            {
                retAttr[SCENE_KEY_CHILD] = childrenAttrs;
            }

            return RCSGetResponse::create(retAttr);
        }
    }
}
